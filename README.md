# ld_preload
LD_PRELOAD allows dynamically linked function hooking.

You might consider LD_PRELOAD a security hole but it is actually secured since it won't work if ruid != euid. 

Shared libraries specified in **/etc/
ld.so.preload** are loaded **before** those from LD_PRELOAD.

Statistically linked libraries are immune to LD_PRELOAD as well as functions defined within the very source code.

#### * [vulnerable.c](/vulnerable.c) is a TOCTTOU vulnerable sourcecoded. <lstat, open> call sequence. 

### Here you can find very useful LD_PRELOAD information:

* http://www.goldsborough.me/c/low-level/kernel/2016/08/29/16-48-53-the_-ld_preload-_trick/
* https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
* https://blog.fpmurphy.com/2012/09/all-about-ld_preload.html
* https://blog.jessfraz.com/post/ld_preload/
* https://stackoverflow.com/questions/426230/what-is-the-ld-preload-trick