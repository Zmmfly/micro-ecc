micro-ecc Command Line Tools
==========
English | [中文](./README.zh_CN.md)

This tool is a command line implementation of `mico-ecc`, which implements ECC secret key generation and signature/checkmark functions, with the default name of `uecc(linux)|uecc.exe(windows)`

Implemented subcommands
- `mkey` generate the key pair for the specified curve
- `dsa` sign or verify a file

Scheduled subcommands
- [ ] `dh`

Example
---------
**Note: Do not use the key pair in the example for practical applications**

### 1. key pair generation
This command generates a secret key pair using the `secp256k1` curve
```
$ uecc mkey -n secp256k1
   Curve name: secp256k1
   Public key: 2844f5e80bf523e2f08e4201cb5857a42b8f6e519e47754b01643e1becd696045cfd298067f32c3899d7e9cb767915f3b75faab4636a507493a7916e8870a683
   Public key: 032844f5e80bf523e2f08e4201cb5857a42b8f6e519e47754b01643e1becd69604 (compressed)
  Private key: 6cd0f7b20b0f3fa74f0699dc01503f783c89555d317e0d33115fb9a8733e85f1
```
Write the public key to the file by adding the parameter `-pub [public key path]`
To write the private key to a file, add the parameter `-prv [private key path]`, which will not output the private key in the console
To change curves, use the `-n [curve name]` parameter

### 2. File signing
#### 2.1 Signing Write Files
This command uses the generated secret key to sign itself, and the signature is written to the sign.bin file
Note: The console log output uses spdlog, the date is hidden in the example
```
$ uecc dsa -s sign.bin --prv prv.bin uecc.exe
[info] file hash with offset(0), length(1262592): b7f1a23da6e4df90e6f1aa7b59cd271dd0515f4440f49e638c75ffac1ab37f31
[info] signature: 9d7811778d38741a8c7ea9c5df370da139d444150606a552cc31e0f5b8e4e2a7063835dc01a5676cf0a4aa9dc343c5041f3a733a88e8aa0fbc0827e0a0a8f0fa
```
This command can also add the `-e` parameter to write the signature to the end of the file being signed.
or specify a signature start offset with the `-o` parameter.
or use the `-l` parameter to specify the length of the signed data from the offset.
If the `-o` argument is not used, the default will be to start at the beginning of the file.
If the `-l` parameter is not used, the length from the start of the offset to the end will be used as the default.
If the `-s` parameter is specified, the specified file will be attempted to be written to when signing, regardless of whether the `-e` parameter is used, and will be cleared if the specified file exists.

#### 2.2 Signature writing to the end of a file
```
$ uecc dsa -e --prv prv.bin uECC.lib
[info] detect lenght: 49770, origin length: 49770 
[info] file hash with offset(0), length(49770): 80f16273ec88dc0567f883082c6232e4f5ad1bc1281df203952d98b02a2087be
[info] signature: 0403184f37c1f641585246b93aa0b48d5545076cd686df9e0c3b65ba62cb02faa2499bd50476aed41d6063a79353b18e40fbbfdcc5e77412b5f24938c3e76653
[info] signature write to end
```

### 3. File signature verification
#### 3.1 Specifying signature file validation
```
$ uecc dsa -v -s sign.bin --pub pub.bin uecc.exe
[info] file hash with offset(0), length(1262592): b7f1a23da6e4df90e6f1aa7b59cd271dd0515f4440f49e638c75ffac1ab37f31
[info] signature verify: pass
```

#### 3.2 Trailing signature verification
```
$ uecc dsa -v -e --pub pub.bin uECC.lib
[info] detect lenght: 49770, origin length: 49834
[info] file hash with offset(0), length(49770): 80f16273ec88dc0567f883082c6232e4f5ad1bc1281df203952d98b02a2087be
[info] signature verify: pass
```