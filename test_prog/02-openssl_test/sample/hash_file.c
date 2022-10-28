#define _CRT_SECURE_NO_WARNINGS
 
#include <stdio.h>
#include <stdlib.h>
#include "openssl/sha.h"
 
void sha256_hash_string(unsigned char* hash, char* outputBuffer) {
	size_t i = 0;
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(outputBuffer + (i * 2), "%02X", hash[i]);
	}
}
 
int calc_sha256(char* filePath, char* output) {
	FILE* file = fopen(filePath, "rb");
	if (!file) {
		return 1;
	}
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
 
	int bufferSize = 1024;
	char* buffer = (char*)malloc(bufferSize * sizeof(char));
	if (buffer == NULL) {
		printf("Failed to invoke malloc function, buffer is NULL.\n");
		return 1;
	}
	int bytesRead = 0;
	while ((bytesRead = fread(buffer, sizeof(char), bufferSize, file))) {
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);
 
	sha256_hash_string(hash, output);
	free(buffer);
	fclose(file);
	return 0;
}
 
int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Please specify a file.\n");
		return 1;
	}
	char* filePath = argv[1];
	char calc_hash[65] = { 0 };
	int rt = calc_sha256(filePath, calc_hash);
	printf("SHA-256: %s\n", calc_hash);
	return rt;
}
