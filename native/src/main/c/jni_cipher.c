#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include "jni_util.h"

typedef unsigned char byte;

JNIEXPORT jlong JNICALL
Java_com_velocitypowered_natives_encryption_OpenSslCipherImpl_init(JNIEnv *env,
    jobject obj,
    jbyteArray key,
    jboolean encrypt)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
        throwException(env, "java/lang/OutOfMemoryError", "allocate cipher");
        return 0;
    }

    jsize keyLen = (*env)->GetArrayLength(env, key);
    jbyte* keyBytes = (*env)->GetPrimitiveArrayCritical(env, key, NULL);
    if (keyBytes == NULL) {
        EVP_CIPHER_CTX_free(ctx);
        throwException(env, "java/lang/OutOfMemoryError", "cipher get key");
        return 0;
    }

    int result = EVP_CipherInit(ctx, EVP_aes_128_cfb8(), (byte*) keyBytes, (byte*) keyBytes,
        encrypt);
    // Release the key byte array now - we won't need it
    (*env)->ReleasePrimitiveArrayCritical(env, key, keyBytes, 0);

    if (result != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throwException(env, "java/security/GeneralSecurityException", "openssl initialize cipher");
        return 0;
    }
    return (jlong) ctx;
}

JNIEXPORT void JNICALL
Java_com_velocitypowered_natives_encryption_OpenSslCipherImpl_free(JNIEnv *env,
    jobject obj,
    jlong ptr)
{
    EVP_CIPHER_CTX_free((EVP_CIPHER_CTX *) ptr);
}

JNIEXPORT void JNICALL
Java_com_velocitypowered_natives_encryption_OpenSslCipherImpl_process(JNIEnv *env,
    jobject obj,
    jlong ptr,
    jlong source,
    jint len,
    jlong dest)
{
    EVP_CipherUpdate((EVP_CIPHER_CTX*) ptr, (byte*) dest, &len, (byte*) source, len);
}