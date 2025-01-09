### libharu库学习

##  学习目的
使用libharu库，把“楷体”里面包含的所有的文字写入到PDF文件中，一页写300个字符，一行写15个字符。

##  学习步骤
  # 第一步下载所需要的库，主要有三个，libharu,linpng和zlib三个库
  zlib: https://github.com/madler/zlib
  libpng: https://github.com/glennrp/libpng
  libharu: https://github.com/libharu/libharu
  然后使用cmake工具依次编译三个库生成我们需要的dll，lib文件。

## 学习环境
  # 使用工具： windows环境下，vs2022.
  使用libharu还是配置相应的设置，在项目目录下新建两个文件夹，一个include和lib文件，再将libharu库和build出来的中的头文件（.h）放到include中，
  lib中放hpdf.lib.还有就是把三个库文件build出来的dll文件放到项目可执行文件中。再在属性配置中添加包含目录include还有库目录lib，还有依赖性hpdf.lib。


  


  

