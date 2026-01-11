# Introduction

HUST PA来自南京大学ICS2019版本，略有修改但是没有很大的变化

## 相关链接

[NJU-ICS2019](https://nju-projectn.github.io/ics-pa-gitbook/ics2019/) 使用这个是因为在很多agent工具上可以使用web相关工具调用。

[HUST-PA-WIKI](https://course.cunok.cn:52443/projects/pa/wiki) hust校园网环境下可以访问，没什么有用的信息，核心问题下面会解释

校园网外访问：

[HUST-PA-WIKI](https://course1.istratus.cn/projects/pa/wiki)

[HUST-PA-DOC](https://course1.istratus.cn/pa/doc2019/)

wiki中没有及时更新git clone地址，应使用如下地址

```bash
git clone https://course1.istratus.cn/pa/ics2019_1.git
```

tip:
- 这是通过阿里云跳转，速度可能不太稳定。
- 文档中提到的网址，要将域名从course.cunok.cn替换成course1.istratus.cn来访问。
- 如果前期已经在校园网内clone，后期要在校园网外访问，则需要手动修改代码根目录.git/config，以及Makefile，将其中的course.cunok.cn都替换成course1.istratus.cn、scripts都替换成scripts1。反之亦然。


## 仓库相关

实际上没有那么多限制，本质就是git仓库，你看下面的log就能得知其实是一个还挺有意思的设计
```bash
- git remote -v

github  git@github.com:Sanstoolow0513/HUST_PA.git (fetch)
github  git@github.com:Sanstoolow0513/HUST_PA.git (push)
hustpa  https://course.cunok.cn:52443/git/U202215561.git (fetch)
hustpa  https://course.cunok.cn:52443/git/U202215561.git (push)
origin  https://course.cunok.cn:52443/pa/ics2019.git (fetch)
origin  https://course.cunok.cn:52443/pa/ics2019.git (push)

- git push -u github --all

Enumerating objects: 2888, done.
Counting objects: 100% (2888/2888), done.
Delta compression using up to 20 threads
Compressing objects: 100% (2539/2539), done.
Writing objects: 100% (2888/2888), 2.96 MiB | 1.49 MiB/s, done.
Total 2888 (delta 1251), reused 1491 (delta 230), pack-reused 0
remote: Resolving deltas: 100% (1251/1251), done.
To github.com:Sanstoolow0513/HUST_PA.git
* [new branch]      master -> master
* [new branch]      pa0 -> pa0
* [new branch]      pa1 -> pa1
* [new branch]      pa2 -> pa2
* [new branch]      pa3 -> pa3
branch 'master' set up to track 'github/master'.
branch 'pa0' set up to track 'github/pa0'.
branch 'pa1' set up to track 'github/pa1'.
branch 'pa2' set up to track 'github/pa2'.
branch 'pa3' set up to track 'github/pa3'.
```

所以日常推送几个如下

```bash
# 推送到 GitHub
git push github
# 推送到 hustpa 服务器
git push hustpa
# 如果设置了默认追踪 （-u语法可以设置），直接 git push 即可
git push
# 推送到 hustpa pa3强制替换(提交作业可以不用make submit)
git checkout pa3
git push -f hustpa pa3
```
