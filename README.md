#php ByteArray
===
[![Build Status](https://secure.travis-ci.org/recoye/php-bytearray.svg?branch=master)](http://travis-ci.org/recoye/php-bytearray)

php版本的ByteArray,like java,as3。

You can send comments, patches, questions [here on github](https://github.com/recoye/php-bytearray/issues) or to mail#recoye.com ([@recoye](http://weibo.com/trco)).

# 安装

~~~
phpize
./configure
make && make install
~~~

`make install` copies `bytearray.so` to an appropriate location, but you still need to enable the module in the PHP config file. To do so, either edit your php.ini with the following contents: `extension=bytearray.so`.

# 函数列表

