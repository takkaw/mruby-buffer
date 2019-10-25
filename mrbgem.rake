MRuby::Gem::Specification.new('mruby-mem') do |spec|
  spec.license = 'MIT'
  spec.author  = 'takkaw'
  spec.summary = 'fix type and fix size memroy allocator'

  spec.add_dependency('mruby-enum-ext',  core: 'mruby-enum-ext')
end
