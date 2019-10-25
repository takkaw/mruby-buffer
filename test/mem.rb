assert("Mem#new") do
  assert_raise { Mem.new 100 }
  assert_nothing_raised { Mem::Int32.new 100 }
  assert_nothing_raised { Mem::Int32.new [1,2,3] }

  assert_nothing_raised {
    10000.times { Mem::Int32.new 100 }
  }

  assert_nothing_raised { Mem::Uint8.new  [1,2,3] }
  assert_nothing_raised { Mem::Int8.new   [1,2,3] }
  assert_nothing_raised { Mem::Uint16.new [1,2,3] }
  assert_nothing_raised { Mem::Int16.new  [1,2,3] }
  assert_nothing_raised { Mem::Uint32.new [1,2,3] }
  assert_nothing_raised { Mem::Int64.new  [1,2,3] }
  assert_nothing_raised { Mem::Uint64.new [1,2,3] }
  skip unless Object.const_defined?(:Float)
  assert_nothing_raised { Mem::Float.new  [1,2,3] }
  assert_nothing_raised { Mem::Double.new [1,2,3] }
end

assert("Mem#inspect") do
  mem = Mem::Int32.new 100
  assert_kind_of String, mem.inspect
end

assert("Mem#type_no") do
  mem = Mem::Int32.new 100
  assert_equal Mem::TYPE.find_index(mem.class), mem.type_no
end

assert("Mem#dim") do
  mem = Mem::Int32.new [1,2,3]
  assert_equal 3, mem.dim
  mem2 = Mem::Int32.new 100
  assert_equal 1, mem2.dim
end

assert("Mem#size") do
  mem = Mem::Int32.new [1,2,3]
  assert_equal 6, mem.size
  mem2 = Mem::Int32.new 100
  assert_equal 100, mem2.size
end

assert("Mem#shape") do
  mem = Mem::Int32.new [1,2,3]
  assert_equal [1,2,3], mem.shape
  mem2 = Mem::Int32.new 100
  assert_equal [100], mem2.shape
end

assert("Mem#[]") do
  mem = Mem::Int8.new [1,2,3]
  assert_equal 100,    (mem[0,0,0] = 100)
  assert_equal 100,     mem[0,0,0]
  assert_equal    128, (mem[0,0,0] = 128)
  assert_equal (-128),  mem[0,0,0]

  mem2 = Mem::Uint8.new [1,2,3]
  assert_equal 256, (mem2[0,0,1] = 256)
  assert_equal 0,    mem2[0,0,1]

  skip unless Object.const_defined?(:Float)
  assert_float 55.5,(mem[0,0,2] = 55.5)
  assert_equal 55,   mem[0,0,2]

  mem3 = Mem::Float.new [1,2,3]
  assert_float 555.5, (mem3[0,0,0] = 555.5)
  assert_float 555.5,  mem3[0,0,0]
  assert_float 10000, (mem3[0,0,1] = 10000)
  assert_float 10000,  mem3[0,0,1]
end

assert("Mem#fill") do
  mem = Mem::Int32.new [1,2,3]
  assert_equal 99, (mem.fill 99)
  assert_equal 99,  mem[0,0,0]
  assert_equal 99,  mem[0,1,2]
end

assert("Mem#dup") do
  mem = Mem::Int32.new [1,2,3]
  mem.fill 99

  mem2 = mem.dup
  assert_equal mem[0,1,2],  mem2[0,1,2]
  assert_equal mem.type_no, mem2.type_no
  assert_equal mem.dim,     mem2.dim
  assert_equal mem.size,    mem2.size
  assert_equal mem.shape,   mem2.shape
end

