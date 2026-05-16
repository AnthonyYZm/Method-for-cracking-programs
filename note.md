# 一个破解程序的基本原理

最近刷到的一个破解程序方法讲解视频，觉得很有趣。

## 示例程序

假设有如下 C 语言程序：

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *password = "abcdefg";

int main() {
    char *pwd = (char *)malloc(100);
    printf("enter password: ");
    scanf("%s", pwd);
    if (strcmp(pwd, password) == 0) {
        printf("success!\n");
    } else {
        printf("incorrect password!\n");
    }
    free(pwd);
    return 0;
}
```

这个程序的逻辑很简单：

1. 用户输入密码；
2. 程序使用 `strcmp()` 将输入内容和内置密码进行比较；
3. 如果比较结果为 `0`，说明两个字符串相同，输出 `success!`；
4. 否则输出 `incorrect password!`。

---

最优雅的方法是使用逆向工程获取密码，对于已编译的文件passwd，可以使用下面的命令查看二进制文件各个段的内容：

```bash
objdump -s passwd
```

在输出中找到 `.rodata` 段后，可以看到程序中硬编码的字符串，例如密码 `"abcdefg"`。

![查看 .rodata 段中的字符串](image.png)

这种方法适用于非常基础的情况。因为示例程序直接把密码以明文形式写在了程序中，所以编译后仍然可以在只读数据段中找到它。

不过，真实程序通常不会这么简单。密码可能经过编码、加密、混淆，或者被拆分存储，因此不一定能直接通过搜索字符串找到。

另一个方法，则是不需要破解密码，可以直接绕过密码检测，观察它是如何决定输出 `success!` 或 `incorrect password!` 的。

首先反汇编程序：

```bash
objdump -d passwd
```

然后找到 `main` 函数对应的汇编代码。

![main 函数反汇编结果](image_2.png)

我们的目标是找到用于密码判断的逻辑部分并对其做出修改，比如类似 `if-else` 的条件判断部分。

假设我们并不熟悉汇编语法。注意到，在`1243` 附近调用了 `strcmp`，紧接着是`124a`处的`jne` ，根据基本的汇编知识，我们知道它表示 **jump if not equal**，也就是“不相等时跳转”。当然也可以继续进行验证，发现它跳转到`125d`，下面的`1267`处调用了`puts`方法，与未发生跳转的`1256`处对应，可以推测是用于输出`success!` 和 `incorrect password!`结果的语句。

既然已经锁定了位置，接下来就可以进行修改。`jne`命令为不相等时跳转，如果我们将其改为相等时跳转，程序的判断逻辑就会被反转。也就是说，输入错误密码时反而能直接进入成功分支。

`jne` 的操作码是 `75`，而 `je` 的操作码是 `74`。

- `jne`：不相等时跳转；
- `je`：相等时跳转。

![跳转指令](image-1.png)

输入命令打开十六进制编辑器：

```bash
hexedit passwd
```

如果是第一次使用需要手动安装：

```bash
sudo apt install hexedit
```

在 `hexedit` 中，可以按 `Enter` 跳转到指定地址。根据前面反汇编结果，找到 `jne` 对应的位置，例如本例中的 `124a`，将操作码`75`改为`74`。

![alt text](image_3.png)

![alt text](image_4.png)

除了把 `jne` 改成 `je`，还可以使用 `NOP` 指令来替换某些跳转逻辑。

`NOP` 的含义是 **No Operation**，也就是“不执行任何操作”。它的机器码通常是90。

再次运行代码，发现输入任意密码都能输出success,成功绕过了密码检测。
![检测结果](image_5.png)

## 参考资料

- [How to Crack any Software](https://youtu.be/FkEh4B5CKfI?si=E6b-tW_3MNO_qP4w)
