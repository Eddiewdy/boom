package boom.exu

import chisel3._
import chisel3.util._

import freechips.rocketchip.config.Parameters
import freechips.rocketchip.rocket._

import boom.common._
import boom.util._

//Enable_PerfCounter_Support
class SubEventCounterIO(readWidth: Int)(implicit p: Parameters) extends BoomBundle
{
  val event_signals  = Input(Vec(16, UInt(4.W)))
  val read_addr = Input(Vec(readWidth, Valid(UInt(4.W))))
  val read_data = Output(Vec(readWidth, UInt(64.W)))
  val write_addr = Input(Vec(readWidth, Valid(UInt(4.W))))
  val write_data = Input(Vec(readWidth, UInt(64.W)))
  val reset_counter = Input(Bool())
}

class SubEventCounter(readWidth: Int)(implicit p: Parameters) extends BoomModule
{
	val io = IO(new SubEventCounterIO(readWidth))
	// val reg_counters = io.event_signals.zipWithIndex.map { case (e, i) => freechips.rocketchip.util.WideCounter(64, e, reset = false) }
  val reg_counters = RegInit(VecInit(Seq.fill(16)(0.U(64.W))))

  for (i <- 0 until readWidth) {
    when (io.read_addr(i).valid) {
      for (w <- 0 until 16) {
        when (io.read_addr(i).bits === w.U) {
          io.read_data(i) := reg_counters(w)
        }
      }
    }
    .otherwise {
      io.read_data(i) := 0.U
    }
    
    // 添加写入计数器的逻辑
    when (io.write_addr(i).valid) {
      for (w <- 0 until 16) {
        when (io.write_addr(i).bits === w.U) {
          reg_counters(w) := io.write_data(i)
          printf("SubEventCounter: Writing to counter %d, value: 0x%x\n", w.U, io.write_data(i))
        }
      }
    }
  }

  when (io.reset_counter) {
    for (w <- 0 until 16) {
      reg_counters(w) := 0.U
    }
  }

  when (RegNext(io.reset_counter)) {
    for (w <- 0 until 16) {
      printf("w: %d, counter: %d\n", w.U, reg_counters(w) )
    }
  }

}


class EventCounterIO(readWidth: Int, signalnum: Int)(implicit p: Parameters) extends BoomBundle
{
  val event_signals  = Input(Vec(signalnum, UInt(4.W)))
  val read_addr = Input(Vec(readWidth, Valid(UInt(7.W))))
  val read_data = Output(Vec(readWidth, UInt(64.W)))
  val write_addr = Input(Vec(readWidth, Valid(UInt(7.W))))
  val write_data = Input(Vec(readWidth, UInt(64.W)))
  val reset_counter = Input(Bool())
}


class EventCounter(readWidth: Int)(implicit p: Parameters) extends BoomModule
{
  
	val io = IO(new EventCounterIO(readWidth, 16*subECounterNum))

  val ecounters = for (w <- 0 until subECounterNum) yield { val e = Module(new SubEventCounter(readWidth)); e }

  //for two cycles delay
	val reg_read_data = Reg(Vec(readWidth, UInt(64.W)))
	for(i <- 0 until readWidth){
		io.read_data(i) := RegNext(reg_read_data(i))
	}

  for (w <- 0 until subECounterNum) {
    ecounters(w).io.reset_counter := io.reset_counter
    for (s <- 0 until 16) {
      ecounters(w).io.event_signals(s) := io.event_signals(s+16*w)
    }

    for (r <- 0 until readWidth) {
      val tag = io.read_addr(r).bits(6, 4)
      ecounters(w).io.read_addr(r).valid := io.read_addr(r).valid && (tag === w.U)
      ecounters(w).io.read_addr(r).bits  := io.read_addr(r).bits(3, 0)
      
      // 添加写入计数器的逻辑
      val write_tag = io.write_addr(r).bits(6, 4)
      ecounters(w).io.write_addr(r).valid := io.write_addr(r).valid && (write_tag === w.U)
      ecounters(w).io.write_addr(r).bits  := io.write_addr(r).bits(3, 0)
      ecounters(w).io.write_data(r) := io.write_data(r)
      
      when (io.write_addr(r).valid && (write_tag === w.U)) {
        printf("EventCounter: Writing to counter with tag %d, idx %d, value: 0x%x\n", 
               write_tag, io.write_addr(r).bits(3, 0), io.write_data(r))
      }
    }
  }

  for (i <- 0 until readWidth) {
    when (io.read_addr(i).valid) {
      val tag = io.read_addr(i).bits(6, 4)
      for (w <- 0 until subECounterNum) {
        when (w.U === tag) {
          reg_read_data(i) := ecounters(w).io.read_data(i)
        }
      }
    }
    .otherwise {
      reg_read_data(i) := 0.U
    }
  }
}
