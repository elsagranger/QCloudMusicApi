﻿#include <QCoreApplication>
#include "Qt-AES/qaesencryption.h"

/**
 * @brief encodeAES 使用AES算法对明文进行加密
 * @param plaintext 明文字符串
 * @param aesKey 密钥字符串
 * @param aesIv 偏移量字符串
 * @return 密文字符串，经过base64编码
 */
QString encodeAES(QString plaintext, QString aesKey, QString aesIv)
{
    // 创建一个AES加密对象，指定AES_128模式，CBC模式和PKCS7填充方式
    QAESEncryption aes(QAESEncryption::AES_128, QAESEncryption::CBC , QAESEncryption::PKCS7);
    // 将明文、密钥和偏移量从QString转换为QByteArray
    QByteArray input(plaintext.toLocal8Bit());
    QByteArray key = aesKey.toLocal8Bit();
    QByteArray iv = aesIv.toLocal8Bit();
    // 调用aes.encode方法对明文进行加密，并将结果转换为base64编码
    QByteArray encodedText = aes.encode(input, key,iv).toBase64();
    // 将加密后的字节数组转换为QString并返回
    QString encoded = encodedText;
    QByteArray encoded1 = encoded.toUtf8();
    return encoded1;
}

/**
 * @brief decodeAES 使用AES算法对密文进行解密
 * @param ciphertext 密文字符串，经过base64编码
 * @param aesKey 密钥字符串
 * @param aesIv 偏移量字符串
 * @return 明文字符串
 */
QString decodeAES(QString ciphertext, QString aesKey, QString aesIv) {
    // 创建一个AES解密对象，指定AES_128模式，CBC模式和PKCS7填充方式
    QAESEncryption aes(QAESEncryption::AES_128, QAESEncryption::CBC , QAESEncryption::PKCS7);
    // 将密文、密钥和偏移量从QString转换为QByteArray
    QByteArray input(ciphertext.toLocal8Bit());
    QByteArray key = aesKey.toLocal8Bit();
    QByteArray iv = aesIv.toLocal8Bit();
    // 调用aes.decode方法对密文进行解密，并将结果从base64编码转换为原始字节数组
    QByteArray decodedText = aes.decode(QByteArray::fromBase64(input), key,iv);
    // 调用aes.removePadding方法去除填充字符
    decodedText = aes.removePadding(decodedText);
    // 将解密后的字节数组转换为QString并返回
    QString decrypted = decodedText;
    QByteArray decrypted1 = decrypted.toUtf8();
    return decrypted1;
}

int main(int argc, char *argv[])
{
    QString aesKey = "0CoJUm6Qyw8W8jud";
    QString aesIv = "0102030405060708";
    auto ciphertext = encodeAES("HelloWorld!", aesKey, aesIv);
    qDebug() << ciphertext << decodeAES(ciphertext, aesKey, aesIv);
}
