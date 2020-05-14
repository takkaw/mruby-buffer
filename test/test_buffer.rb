assert("Buffer.allocate") do
  assert_nothing_raised { Buffer::Uint32.allocate }
end

assert("Buffer#new") do
  assert_raise { Buffer.new 100 }
  assert_nothing_raised { Buffer::Int32.new 100 }
  assert_nothing_raised { Buffer::Int32.new [1,2,3] }

  assert_nothing_raised {
    10000.times { Buffer::Int32.new 100 }
  }

  assert_nothing_raised { Buffer::Uint8.new  [1,2,3] }
  assert_nothing_raised { Buffer::Int8.new   [1,2,3] }
  assert_nothing_raised { Buffer::Uint16.new [1,2,3] }
  assert_nothing_raised { Buffer::Int16.new  [1,2,3] }
  assert_nothing_raised { Buffer::Uint32.new [1,2,3] }
  assert_nothing_raised { Buffer::Int64.new  [1,2,3] }
  assert_nothing_raised { Buffer::Uint64.new [1,2,3] }
  skip unless Object.const_defined?(:Float)
  assert_nothing_raised { Buffer::Float.new  [1,2,3] }
  assert_nothing_raised { Buffer::Double.new [1,2,3] }
end

assert("Buffer#inspect") do
  buffer = Buffer::Int32.new 100
  assert_kind_of String, buffer.inspect
end

assert("Buffer#dim") do
  buffer = Buffer::Int32.new [1,2,3]
  assert_equal 3, buffer.dim
  buffer2 = Buffer::Int32.new 100
  assert_equal 1, buffer2.dim
end

assert("Buffer#size") do
  buffer = Buffer::Int32.new [1,2,3]
  assert_equal 6, buffer.size
  buffer2 = Buffer::Int32.new 100
  assert_equal 100, buffer2.size
end

assert("Buffer#shape") do
  buffer = Buffer::Int32.new [1,2,3]
  assert_equal [1,2,3], buffer.shape
  buffer2 = Buffer::Int32.new 100
  assert_equal [100], buffer2.shape
end

assert("Buffer#[]") do
  buffer = Buffer::Int8.new [1,2,3]
  assert_equal 100,    (buffer[0,0,0] = 100)
  assert_equal 100,     buffer[0,0,0]
  assert_equal    128, (buffer[0,0,0] = 128)
  assert_equal (-128),  buffer[0,0,0]

  buffer2 = Buffer::Uint8.new [1,2,3]
  assert_equal 256, (buffer2[0,0,1] = 256)
  assert_equal 0,    buffer2[0,0,1]

  skip unless Object.const_defined?(:Float)
  assert_float 55.5,(buffer[0,0,2] = 55.5)
  assert_equal 55,   buffer[0,0,2]

  buffer3 = Buffer::Float.new [1,2,3]
  assert_float 555.5, (buffer3[0,0,0] = 555.5)
  assert_float 555.5,  buffer3[0,0,0]
  assert_float 10000, (buffer3[0,0,1] = 10000)
  assert_float 10000,  buffer3[0,0,1]
end

assert("Buffer#fill") do
  buffer = Buffer::Int32.new [1,2,3]
  assert_equal 99, (buffer.fill 99)
  assert_equal 99,  buffer[0,0,0]
  assert_equal 99,  buffer[0,1,2]
end

assert("Buffer#dup") do
  buffer = Buffer::Int32.new [1,2,3]
  buffer.fill 99

  buffer2 = buffer.dup
  assert_equal buffer[0,1,2],  buffer2[0,1,2]
  assert_equal buffer.dim,     buffer2.dim
  assert_equal buffer.size,    buffer2.size
  assert_equal buffer.shape,   buffer2.shape
  assert_equal buffer.class,   buffer2.class
  assert_not_equal buffer.object_id, buffer2.object_id
end

