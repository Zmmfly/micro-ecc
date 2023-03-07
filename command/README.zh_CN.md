micro-ecc 命令行工具
==========
[English](./README.md) | 中文

该工具为`mico-ecc`命令行实现，实现ECC秘钥生成与签名/验签等功能，默认名称为`uecc(linux)|uecc.exe(windows)`

已实现子命令
- `mkey` 生成指定曲线的密钥对
- `dsa` 对文件进行签名或验签

计划子命令
- [ ] `dh`

示例
---------
**注意：请勿使用示例中的密钥对用于实际应用**

### 1. 密钥对生成
该命令使用`secp256k1`曲线生成秘钥对
```
$ uecc mkey -n secp256k1
   Curve name: secp256k1
   Public key: 2844f5e80bf523e2f08e4201cb5857a42b8f6e519e47754b01643e1becd696045cfd298067f32c3899d7e9cb767915f3b75faab4636a507493a7916e8870a683
   Public key: 032844f5e80bf523e2f08e4201cb5857a42b8f6e519e47754b01643e1becd69604 (compressed)
  Private key: 6cd0f7b20b0f3fa74f0699dc01503f783c89555d317e0d33115fb9a8733e85f1
```
将公钥写入文件可增加参数`--pub [public key path]`
将私钥写入文件可增加参数`--prv [private key path]`，使用该参数后，私钥将不会在控制台进行输出
更换曲线可使用 `-n [curve name]`参数

### 2. 文件签名
#### 2.1 签名写入文件
该命令使用生成的秘钥对自身进行签名，签名写入sign.bin文件
注意：控制台日志输出使用spdlog，示例中已隐去日期
```
$ uecc dsa -s sign.bin --prv prv.bin uecc.exe
[info] file hash with offset(0), length(1262592): b7f1a23da6e4df90e6f1aa7b59cd271dd0515f4440f49e638c75ffac1ab37f31
[info] signature: 9d7811778d38741a8c7ea9c5df370da139d444150606a552cc31e0f5b8e4e2a7063835dc01a5676cf0a4aa9dc343c5041f3a733a88e8aa0fbc0827e0a0a8f0fa
```
该命令还可添加`-e`参数，将签名写入被签名文件尾部；
或是使用`-o`参数指定签名起始偏移；
或是使用`-l`参数指定从偏移开始的被签名数据长度；
若不使用`-o`参数，将默认从文件头开始；
若不使用`-l`参数，默认使用从偏移开始至尾部作为长度。
若指定`-s`参数，无论是否使用`-e`参数，在签名时均将尝试写入指定文件，若指定文件存在，将被清空。

#### 2.2 签名写入文件结尾
```
$ uecc dsa -e --prv prv.bin uECC.lib
[info] detect lenght: 49770, origin length: 49770 
[info] file hash with offset(0), length(49770): 80f16273ec88dc0567f883082c6232e4f5ad1bc1281df203952d98b02a2087be
[info] signature: 0403184f37c1f641585246b93aa0b48d5545076cd686df9e0c3b65ba62cb02faa2499bd50476aed41d6063a79353b18e40fbbfdcc5e77412b5f24938c3e76653
[info] signature write to end
```

### 3. 文件验签
#### 3.1 指定签名文件验签
```
$ uecc dsa -v -s sign.bin --pub pub.bin uecc.exe
[info] file hash with offset(0), length(1262592): b7f1a23da6e4df90e6f1aa7b59cd271dd0515f4440f49e638c75ffac1ab37f31
[info] signature verify: pass
```

#### 3.2 尾部签名验签
```
$ uecc dsa -v -e --pub pub.bin uECC.lib
[info] detect lenght: 49770, origin length: 49834
[info] file hash with offset(0), length(49770): 80f16273ec88dc0567f883082c6232e4f5ad1bc1281df203952d98b02a2087be
[info] signature verify: pass
```
