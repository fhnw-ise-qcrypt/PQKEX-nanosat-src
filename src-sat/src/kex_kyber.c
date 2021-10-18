/**
 * @file   kex_kyber.c
 * @brief  implements the kyber512 algorithm 
 *         using file IO and the GOSH command system
 * @author Simon Burkhardt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include <FreeRTOS.h>
#include <task.h>

#include <command/command.h>

#include <util/color_printf.h>

#include "kem.h"
#include "kex.h"

#include "kex_kyber.h"

uint8_t pka[CRYPTO_PUBLICKEYBYTES];
uint8_t pkb[CRYPTO_PUBLICKEYBYTES];
uint8_t ska[CRYPTO_SECRETKEYBYTES];
uint8_t ake_senda[KEX_AKE_SENDABYTES];
uint8_t ake_sendb[KEX_AKE_SENDBBYTES];
uint8_t eska[CRYPTO_SECRETKEYBYTES];
uint8_t tk[KEX_SSBYTES];
uint8_t ka[KEX_SSBYTES];

/**
 * @brief function to read a byte string from a txt file with bytes encoded in hexadecimal
 * @note  implementation using "fread()" which works on avr-32 
 * @param filename pointer to char string containing the path to the file to be read
 * @param out pointer to output array to store the bytes in
 * @param bytes number of expected bytes to be read from the file
 * */
int readHexFile(char *filename, uint8_t *out, size_t bytes){
  printf("reading\n");
  FILE *fp;
  fp = fopen(filename, "r");
  if( fp != NULL ){
    int i;
    char line[2*bytes+1];
    size_t len = 0;
    len = fread(line, sizeof(char), 2*bytes, fp);
    if(len < 2*bytes){
      fprintf(stderr, "[error] %s does not contain the required amout of bytes (%d/%d)\n", filename, (int)len/2, (int)bytes);
      return -2;
    }
    for(i=0;(i<(int)bytes);i++){
      char tmp[3] = {line[2*i], line[2*i+1], 0};
      out[i] = (uint8_t)strtol(tmp,NULL,16);
    }
    fclose(fp);
    printf("[ ok  ] read %d / %d bytes from %s\n", (int)len/2, (int)bytes, filename);
  } else {
    fclose(fp);
    fprintf(stderr, "[error] file: %s does not exist\n", filename);
    return -1;
  }
  return 0;
}

/**
 * @brief creates and writes a byte array encoded in hex to a new file
 * @param filename pointer to char string containing the path to the file to be written
 * @param in pointer to input array that should be written to the file
 * @param bytes length of the input array
 * */
int writeHexFile(char *filename, uint8_t *in, size_t bytes){
  int i;
  FILE *fp;
  fp = fopen(filename, "w+");
  for(i=0;i<(int)bytes;i++){
    fprintf(fp,"%02x", in[i]);
  }
  fclose(fp);
  printf("[ ok  ] wrote %d bytes to %s\n", (int)bytes, filename);
  return 0;
}

/**
 * @brief  generates a symmetric keypair and writes it to 2 files 
 */
int cmd_kex_kyber_init(struct command_context *ctx) {
	int i;
	for(i=0;i<CRYPTO_PUBLICKEYBYTES;i++){pkb[i]=0;pka[i]=0;}
	for(i=0;i<CRYPTO_SECRETKEYBYTES;i++){ska[i]=0;eska[i]=0;}
	for(i=0;i<KEX_AKE_SENDABYTES;i++){ake_senda[i]=0;}
	for(i=0;i<KEX_AKE_SENDBBYTES;i++){ake_sendb[i]=0;}
	for(i=0;i<KEX_SSBYTES;i++){tk[i]=0;ka[i]=0;}

	crypto_kem_keypair(pka, ska); // Generate static keypair for Alice
	writeHexFile(KEX_FILE_SKA, ska, CRYPTO_SECRETKEYBYTES);
	writeHexFile(KEX_FILE_PKA, pka, CRYPTO_PUBLICKEYBYTES);

	return CMD_ERROR_NONE;
}

/**
 *  @brief  reads the published public key from the other party into an array
 */
int cmd_kex_kyber_pub(struct command_context *ctx) {
	FILE *fp;
	fp = fopen(KEX_FILE_PKB, "r");
	if( fp != NULL ){
		readHexFile(KEX_FILE_PKB, pkb, CRYPTO_PUBLICKEYBYTES);
	} else {
		printf("%s not found\n", KEX_FILE_PKB);
		return CMD_ERROR_FAIL;
	}
	fclose(fp);

	return CMD_ERROR_NONE;
}

/**
 * @brief initialize the key exchange here and send ake_senda.txt to GND
 * */
int cmd_kex_kyber_initA(struct command_context *ctx) {
	// todo: verify that ESKA / TK are empty
	// todo: verify that PKB is non empty
	//cmd_kex_kyber_pub(ctx);

	kex_ake_initA(ake_senda, tk, eska, pkb);
	
	writeHexFile(KEX_FILE_SENDA, ake_senda, KEX_AKE_SENDABYTES);
	writeHexFile(KEX_FILE_ESKA, eska, CRYPTO_SECRETKEYBYTES);
	writeHexFile(KEX_FILE_TK, tk, KEX_SSBYTES);

	printf("Send %s to other party\n", KEX_FILE_SENDA);
	return CMD_ERROR_NONE;
}

/**
 * @brief  continues the key exchange that was started on the ground
 * @note the GND initialized key exchange and sent "ake_senda.txt" to here
 * process the ake_senda here
 * */
int cmd_kex_kyber_sharedB(struct command_context *ctx) {
	readHexFile(KEX_FILE_SENDA, ake_senda, KEX_AKE_SENDABYTES);

  // load from files in case of restart of OBC
	if( (ska[0]==0) && (ska[1]==0) && (ska[2]==0) &&
		(pkb[0]==0) && (pkb[1]==0) && (pkb[2]==0)){
		FILE *fp;
		fp = fopen(KEX_FILE_SKA, "r");
		if( fp != NULL ){
			readHexFile(KEX_FILE_SKA, ska, CRYPTO_SECRETKEYBYTES);
		} 
    fclose(fp);
		fp = fopen(KEX_FILE_PKB, "r");
		if( fp != NULL ){
			readHexFile(KEX_FILE_PKB, pkb, CRYPTO_PUBLICKEYBYTES);
		} 
    fclose(fp);
  }

	kex_ake_sharedB(ake_sendb, ka, ake_senda, ska, pkb); // Run by Bob
	writeHexFile(KEX_FILE_COMMON, ka, KEX_SSBYTES);
	writeHexFile(KEX_FILE_SENDB, ake_sendb, KEX_AKE_SENDBBYTES);
	printf("Send %s to other party\n", KEX_FILE_SENDA);

	return CMD_ERROR_NONE;
}

/**
 * @brief finish up the key exchange
 * */
int cmd_kex_kyber_sharedA(struct command_context *ctx) {
	readHexFile(KEX_FILE_SENDB, ake_sendb, KEX_AKE_SENDBBYTES);

	// load from files in case of restart of OBC
	if( (tk[0]==0) && (tk[1]==0) && (tk[2]==0) &&
		(eska[0]==0) && (eska[1]==0) && (eska[2]==0)){
		FILE *fp;
		fp = fopen(KEX_FILE_TK, "r");
		if( fp != NULL ){
			readHexFile(KEX_FILE_TK, tk, KEX_SSBYTES);
		} 
    fclose(fp);
		fp = fopen(KEX_FILE_ESKA, "r");
		if( fp != NULL ){
			readHexFile(KEX_FILE_ESKA, eska, CRYPTO_SECRETKEYBYTES);
		} 
    fclose(fp);
    fp = fopen(KEX_FILE_SKA, "r");
    if( fp != NULL ){
      readHexFile(KEX_FILE_SKA, ska, CRYPTO_SECRETKEYBYTES);
    } 
    fclose(fp);
	}

	kex_ake_sharedA(ka, ake_sendb, tk, eska, ska); // Run by Alice
	writeHexFile(KEX_FILE_COMMON, ka, KEX_SSBYTES);

	return CMD_ERROR_NONE;
}

command_t  __sub_command kex_kyber_subcommands[] = {
	{
		.name = "-i",
		.help = "Initialize static key pair",
		.handler = cmd_kex_kyber_init,
	},{
		.name = "-p",
		.help = "Set remote party's public key",
		.handler = cmd_kex_kyber_pub,
		.usage = "<key>",
	},{
		.name = "-A",
		.help = "Initiate key exchange",
		.handler = cmd_kex_kyber_initA,
	},{
		.name = "-B",
		.help = "Continue key exchange",
		.handler = cmd_kex_kyber_sharedB,
		.usage = "<message>",
	},{
		.name = "-C",
		.help = "Finish key exchange",
		.handler = cmd_kex_kyber_sharedA,
		.usage = "<message>",
	},
};

command_t __root_command kex_kyber_commands[] = {
	{
		.name = "kex_kyb",
		.help = "Kyber512 KEX commands",
		.chain = INIT_CHAIN(kex_kyber_subcommands),
	}
};

/**
 * @brief initializes the arrays to 0 and registers GOSH commands
 * @note  this will run when the OBC is restarted.
 *        other functions can detect, when a temporary array is 0 
 *        and read the files into those arrays again.
 */
void cmd_kex_kyber_setup(void) {
	command_register(kex_kyber_commands);
	int i;
	for(i=0;i<CRYPTO_SECRETKEYBYTES;i++){eska[i]=0;}
	for(i=0;i<KEX_SSBYTES;i++){tk[i]=0;}
}


