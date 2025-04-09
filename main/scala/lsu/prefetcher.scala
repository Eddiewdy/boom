//******************************************************************************
// See LICENSE.Berkeley for license details.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

package boom.lsu

import boom.common._
import boom.common.constants.MemoryOpConstants
import chisel3._
import chisel3.util._
import freechips.rocketchip.config.Parameters
import freechips.rocketchip.diplomacy._
import freechips.rocketchip.rocket._
import freechips.rocketchip.tile._
import freechips.rocketchip.tilelink._
import freechips.rocketchip.util._



abstract class DataPrefetcher(implicit edge: TLEdgeOut, p: Parameters) extends BoomModule()(p)
{
  val io = IO(new Bundle {
    val mshr_avail = Input(Bool())
    val req_val    = Input(Bool())
    val req_addr   = Input(UInt(coreMaxAddrBits.W))
    val req_vaddr  = Input(UInt(coreMaxAddrBits.W))
    val req_coh    = Input(new ClientMetadata)
    val malloc_obj_addr = Input(UInt(coreMaxAddrBits.W))
    val malloc_obj_size = Input(UInt(coreMaxAddrBits.W))
    val malloc_obj_id   = Input(UInt(5.W))

    val prefetch   = Decoupled(new BoomDCacheReq)
    
    // TLB翻译接口
    val prefetch_translation_req = new DecoupledIO(new BoomDCacheTranslationReq)
    val prefetch_translation_resp = Flipped(new DecoupledIO(new BoomDCacheTranslationResp))
  })
}

/**
  * Does not prefetch
  */
class NullPrefetcher(implicit edge: TLEdgeOut, p: Parameters) extends DataPrefetcher
{
  io.prefetch.valid := false.B
  io.prefetch.bits  := DontCare
  
  // 默认不需要进行地址翻译
  io.prefetch_translation_req.valid := false.B
  io.prefetch_translation_req.bits.translation_vaddr := DontCare
}

/**
  * Next line prefetcher. Grabs the next line on a cache miss
  */
class NLPrefetcher(implicit edge: TLEdgeOut, p: Parameters) extends DataPrefetcher
{

  val req_valid = RegInit(false.B)
  val req_addr  = Reg(UInt(coreMaxAddrBits.W))
  val req_vaddr = Reg(UInt(coreMaxAddrBits.W))
  val req_cmd   = Reg(UInt(M_SZ.W))

  val mshr_req_addr = io.req_addr + cacheBlockBytes.U
  val mshr_req_vaddr = io.req_vaddr + cacheBlockBytes.U
  val cacheable = edge.manager.supportsAcquireBSafe(mshr_req_addr, lgCacheBlockBytes.U)
  when (io.req_val && cacheable) {
    req_valid := true.B
    req_addr  := mshr_req_addr
    req_vaddr := mshr_req_vaddr
    req_cmd   := Mux(ClientStates.hasWritePermission(io.req_coh.state), M_PFW, M_PFR)
  } .elsewhen (io.prefetch.fire()) {
    req_valid := false.B
  }

  io.prefetch.valid            := req_valid && io.mshr_avail
  io.prefetch.bits.addr        := req_addr
  io.prefetch.bits.uop         := NullMicroOp
  io.prefetch.bits.uop.mem_cmd := req_cmd
  io.prefetch.bits.data        := DontCare
  io.prefetch.bits.vaddr       := req_vaddr  // 添加虚拟地址
  
  // 不需要TLB翻译，因为已经使用物理地址
  io.prefetch_translation_req.valid := false.B
  io.prefetch_translation_req.bits.translation_vaddr := DontCare
}

/**
  * L2-Only Next Line Prefetcher
  * 
  * 这个预取器生成的请求只会将数据预取到L2缓存，不会填充L1缓存
  * 通过设置特殊的mem_cmd实现 - M_PFR_L2ONLY
  */
class L2OnlyNLPrefetcher(implicit edge: TLEdgeOut, p: Parameters) extends DataPrefetcher
  with MemoryOpConstants
{
  val req_valid = RegInit(false.B)
  val req_addr  = Reg(UInt(coreMaxAddrBits.W))
  val req_vaddr = Reg(UInt(coreMaxAddrBits.W))
  val req_cmd   = Reg(UInt(M_SZ.W))

  val mshr_req_addr = io.req_addr + cacheBlockBytes.U
  val mshr_req_vaddr = io.req_vaddr + cacheBlockBytes.U
  val cacheable = edge.manager.supportsAcquireBSafe(mshr_req_addr, lgCacheBlockBytes.U)
  when (io.req_val && cacheable) {
    req_valid := true.B
    req_addr  := mshr_req_addr
    req_vaddr := mshr_req_vaddr
    // 使用特殊的预取命令，指示只预取到L2
    req_cmd   := M_PFR_L2ONLY  // 使用在MemoryOpConstants中定义的命令
  } .elsewhen (io.prefetch.fire()) {
    req_valid := false.B
  }

  io.prefetch.valid            := req_valid && io.mshr_avail
  io.prefetch.bits.addr        := req_addr
  io.prefetch.bits.uop         := NullMicroOp
  io.prefetch.bits.uop.mem_cmd := req_cmd
  io.prefetch.bits.data        := DontCare
  io.prefetch.bits.vaddr       := req_vaddr
  
  // 不需要TLB翻译，因为已经使用物理地址
  io.prefetch_translation_req.valid := false.B
  io.prefetch_translation_req.bits.translation_vaddr := DontCare
}

/**
  * Virtual Address based Next Line Prefetcher
  * Uses virtual addresses for prefetching and requires TLB translation
  */
class VAddrNLPrefetcher(implicit edge: TLEdgeOut, p: Parameters) extends DataPrefetcher
{
  val req_valid = RegInit(false.B)
  val req_vaddr = Reg(UInt(coreMaxAddrBits.W))
  val req_paddr = Reg(UInt(coreMaxAddrBits.W))
  val req_cmd   = Reg(UInt(M_SZ.W))
  val needs_translation = RegInit(false.B)
  
  // 计算下一行的虚拟地址
  val next_vaddr = io.req_vaddr + cacheBlockBytes.U

  // 当收到有效请求时，准备预取下一行
  when (io.req_val) {
    req_valid := true.B
    req_vaddr := next_vaddr
    req_cmd   := Mux(ClientStates.hasWritePermission(io.req_coh.state), M_PFW, M_PFR)
    needs_translation := true.B
  } .elsewhen (io.prefetch.fire()) {
    req_valid := false.B
  }
  
  // 处理翻译结果
  when (io.prefetch_translation_resp.valid && !io.prefetch_translation_resp.bits.translation_miss) {
    req_paddr := io.prefetch_translation_resp.bits.translation_paddr
    needs_translation := false.B
  } .elsewhen (io.prefetch_translation_resp.valid && io.prefetch_translation_resp.bits.translation_miss) {
    // 翻译失败，取消预取
    req_valid := false.B
    needs_translation := false.B
  }

  // 请求地址翻译
  io.prefetch_translation_req.valid := req_valid && needs_translation
  io.prefetch_translation_req.bits.translation_vaddr := req_vaddr

  // 预取请求的输出
  io.prefetch.valid            := req_valid && io.mshr_avail && !needs_translation
  io.prefetch.bits.addr        := req_paddr  // 使用翻译后的物理地址
  io.prefetch.bits.uop         := NullMicroOp
  io.prefetch.bits.uop.mem_cmd := req_cmd
  io.prefetch.bits.data        := DontCare
  io.prefetch.bits.vaddr       := req_vaddr  // 保留原始虚拟地址
}


