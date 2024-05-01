/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 12/3/18 1:32 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#include "symmetric_encryption.h"
#include "logs.h"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

/* Set up the key and iv. Do I need to say to not hard code these in a
 * real application? :-)
 */

/* A 256 bit key */
static unsigned char *KEY = (unsigned char *)"01234567890123456789012345678901";
/* A 128 bit IV */
static unsigned char *IV = (unsigned char *)"0123456789012345";

//static inline int handleErrors(void)
//{
////	ERR_print_errors_fp(stderr);
////	abort();
//	return -1;
//}

int encrypt(const char *plaintext, unsigned char *ciphertext, int *ciphertext_len)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len = (int)strlen(plaintext);

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		LOGI("");
		return -1;
	}

	/* Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, KEY, IV)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	*ciphertext_len = len;

	/* Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}

	*ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return 0;
}

int encrypt_string_to_file(const char *message, const char *filepath)
{
	int cipher_len;
	unsigned char cipher_text[1024];
	FILE *file;

	if (0 != encrypt(message, cipher_text, &cipher_len)) {
		LOGI("encrypt message failure.");
		return -1;
	}

	file = fopen(filepath, "wb");

	if (file == NULL) {
		LOGI("");
		return -1;
	}

	size_t len = fwrite(cipher_text, (size_t)cipher_len, 1, file);
	if (1 != len) {
		LOGI("Save encrypt message failure. Ciphertext_Len = %zu, write len = %zu", cipher_len, len);
		return -1;
	}

	fclose(file);

	return 0;
}

int decrypt(unsigned char *cipher_text, int cipher_text_len, unsigned char *plaintext, int *plain_text_len)
{
	EVP_CIPHER_CTX *ctx;
	int len;

	/* Create and initialise the context */
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		LOGI("");
		return -1;
	}

	/* Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits */
	if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, KEY, IV)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, cipher_text, cipher_text_len)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	*plain_text_len = len;

	/* Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
		LOGI("");
		EVP_CIPHER_CTX_free(ctx);
		return -1;
	}
	*plain_text_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return 0;
}

int decrypt_string_from_file(const char *filepath, char *message, size_t *message_len)
{
	unsigned char ciphertext[1024];

	FILE *file;

	file = fopen(filepath, "rb");
	if (file == NULL) {
		LOGI("");
		return -1;
	}

	size_t ciphertext_len = fread(ciphertext, 1, sizeof(ciphertext), file);
	if (ciphertext_len <= 0) {
		LOGI("Read encrypt message failure.");
		return -1;
	}

	fclose(file);

	if (0 != decrypt(ciphertext, (int)ciphertext_len, (unsigned char *)message, (int *)message_len)) {
		LOGI("decrypt message failure.");
		return -1;
	}

	return 0;
}


int symmetric_encryption_test1(void)
{
	const char *plaintext = "Hello, world!";
	char decrypt_text[1024];
	size_t decrypt_text_len;

	LOGI("Source message[%zu]: %s", strlen(plaintext), plaintext);

	if (0 != encrypt_string_to_file(plaintext, ".endpoint.bin")) {
		return -1;
	}

	if (0 != decrypt_string_from_file(".endpoint.bin", decrypt_text, &decrypt_text_len)){
		return -1;
	}

	LOGI("Decrypt message[%zu]: %s", decrypt_text_len, decrypt_text);
}

/*

int symmetric_encryption_test(void)
{
	*/
/* Set up the key and iv. Do I need to say to not hard code these in a
	 * real application? :-)
	 *//*


	*/
/* A 256 bit key *//*

	unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
	unsigned char *kee = (unsigned char *)"01234567890123456789012345678902";
	*/
/* A 128 bit IV *//*

	unsigned char *iv = (unsigned char *)"0123456789012345";

	*/
/* Message to be encrypted *//*

	unsigned char *plaintext =
		(unsigned char *)"The quick brown fox jumps over the lazy dog";

	*/
/* Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, dependant on the
	 * algorithm and mode
	 *//*

	unsigned char ciphertext[128];

	*/
/* Buffer for the decrypted text *//*

	unsigned char decryptedtext[128];

	int decryptedtext_len, ciphertext_len;
	*/
/* Encrypt the plaintext *//*

	ciphertext_len = encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);

	*/
/* Do something useful with the ciphertext here *//*

	printf("Ciphertext is:\n");
	BIO_dump_fp(stdout, (const char *)ciphertext, ciphertext_len);

	*/
/* Decrypt the ciphertext *//*

	decryptedtext_len = decrypt(ciphertext, ciphertext_len, kee, iv, decryptedtext);

	*/
/* Add a NULL terminator. We are expecting printable text *//*

	decryptedtext[decryptedtext_len] = '\0';

	*/
/* Show the decrypted text *//*

	printf("Decrypted text is:\n");
	printf("%s\n", decryptedtext);


	return 0;
}*/
