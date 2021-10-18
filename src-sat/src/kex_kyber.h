/**
 * @file   kex_kyber.h
 * @brief  implements the kyber512 algorithm 
 *         using file IO and the GOSH command system
 * @author Simon Burkhardt
 */

#ifndef KEX_KYBER_H
#define KEX_KYBER_H

#define KEX_FILE_SENDA "/flash/ake_senda.txt"
#define KEX_FILE_SENDB "/flash/ake_sendb.txt"
#define KEX_FILE_SKA "/flash/SKA.key"
#define KEX_FILE_PKA "/flash/PKA.key"
#define KEX_FILE_PKB "/flash/PKB.key"
#define KEX_FILE_ESKA "/flash/eska.key"
#define KEX_FILE_TK "/flash/tk.key"

#define KEX_FILE_SKA "/flash/SKA.key"
#define KEX_FILE_SKB "/flash/SKB.key"
#define KEX_FILE_COMMON "/flash/COMMON.key"

void cmd_kex_kyber_setup(void);

int readHexFile(char *filename, uint8_t *out, size_t bytes);
int writeHexFile(char *filename, uint8_t *in, size_t bytes);

/*
int cmd_kex_kyber_init(struct command_context *ctx);
int cmd_kex_kyber_pub(struct command_context *ctx);
int cmd_kex_kyber_initA(struct command_context *ctx);
int cmd_kex_kyber_sharedB(struct command_context *ctx);
int cmd_kex_kyber_sharedA(struct command_context *ctx);
int cmd_kex_kyber_show_pka(struct command_context *ctx);
int cmd_kex_kyber_show_ska(struct command_context *ctx);
int cmd_kex_kyber_show_pkb(struct command_context *ctx);
int cmd_kex_kyber_show_key(struct command_context *ctx);
*/

#endif // KEX_KYBER_H 
