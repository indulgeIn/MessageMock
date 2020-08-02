Pod::Spec.new do |s|

  s.name         = "MessageMock"

  s.version      = "1.0"

  s.summary      = "Objective-C 方法模拟工具"

  s.description  = <<-DESC
			Objective-C 方法模拟工具
                   DESC

  s.homepage     = "https://github.com/indulgeIn"

  s.license      = "MIT"

  s.author       = { "indulgeIn" => "1106355439@qq.com" }

  s.platform     = :ios, "9.0"

  s.source       = { :git => "https://github.com/indulgeIn/MessageMock.git", :tag => "#{s.version}" }

  s.requires_arc = true

  s.source_files   = "MessageMock/**/*.{h,m,mm,c,cpp,hpp}"

  s.libraries = 'c++.1'

end
