assert("CArray#new") do
  assert_raise { CArray.new 100 }
  assert_nothing_raised { CArray::Float.new 100 }
  assert_nothing_raised { CArray::Float.new [1,2,3] }

  assert_nothing_raised {
    10000.times { CArray::Float.new 100 }
  }

  assert_nothing_raised { CArray::Uint8.new  [1,2,3] }
  assert_nothing_raised { CArray::Int8.new   [1,2,3] }
  assert_nothing_raised { CArray::Uint16.new [1,2,3] }
  assert_nothing_raised { CArray::Int16.new  [1,2,3] }
  assert_nothing_raised { CArray::Uint32.new [1,2,3] }
  assert_nothing_raised { CArray::Int64.new  [1,2,3] }
  assert_nothing_raised { CArray::Uint64.new [1,2,3] }
  assert_nothing_raised { CArray::Float.new  [1,2,3] }
  assert_nothing_raised { CArray::Double.new [1,2,3] }
end

assert("CArray#inspect") do
  carray = CArray::Float.new 100
  assert_kind_of String, carray.inspect
end

assert("CArray#type_no") do
  carray = CArray::Float.new 100
  assert_equal CArray::TYPE.find_index(carray.class), carray.type_no
end

assert("CArray#dim") do
  carray = CArray::Float.new [1,2,3]
  assert_equal 3, carray.dim
  carray2 = CArray::Float.new 100
  assert_equal 1, carray2.dim
end

assert("CArray#size") do
  carray = CArray::Float.new [1,2,3]
  assert_equal 6, carray.size
  carray2 = CArray::Float.new 100
  assert_equal 100, carray2.size
end

assert("CArray#shape") do
  carray = CArray::Float.new [1,2,3]
  assert_equal [1,2,3], carray.shape
  carray2 = CArray::Float.new 100
  assert_equal [100], carray2.shape
end

assert("CArray#[]") do
  carray = CArray::Float.new [1,2,3]
  assert_float 555.5, (carray[0,0,0] = 555.5)
  assert_float 555.5,  carray[0,0,0]
  assert_float 10000, (carray[0,0,1] = 10000)
  assert_float 10000,  carray[0,0,1]

  carray2 = CArray::Uint8.new [1,2,3]
  assert_equal 100, (carray2[0,0,0] = 100)
  assert_equal 100,  carray2[0,0,0]
  assert_equal 256, (carray2[0,0,1] = 256)
  assert_equal 0,    carray2[0,0,1]
  assert_float 55.5,(carray2[0,0,2] = 55.5)
  assert_equal 55,   carray2[0,0,2]

  carray3 = CArray::Int8.new [1,2,3]
  assert_equal    128, (carray3[0,0,0] = 128)
  assert_equal (-128), carray3[0,0,0]
end

assert("CArray#fill") do
  carray = CArray::Float.new [1,2,3]
  assert_equal 99, (carray.fill 99)
  assert_equal 99,  carray[0,0,0]
  assert_equal 99,  carray[0,1,2]
end

assert("CArray#dup") do
  carray  = CArray::Float.new [1,2,3]
  carray.fill 99

  carray2 = carray.dup
  assert_equal carray[0,1,2], carray2[0,1,2]
  assert_equal carray.type_no, carray2.type_no
  assert_equal carray.dim, carray2.dim
  assert_equal carray.size, carray2.size
  assert_equal carray.shape, carray2.shape
end

