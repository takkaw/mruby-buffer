MRuby::Gem::Specification.new('mruby-carray') do |spec|
  spec.license = 'MIT'
  spec.author  = 'takkaw'
  spec.summary = 'C numeric data array'

  spec.add_dependency('mruby-enum-ext',  core: 'mruby-enum-ext')
end
