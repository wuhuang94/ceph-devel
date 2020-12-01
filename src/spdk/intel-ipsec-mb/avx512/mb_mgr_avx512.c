/*******************************************************************************
 Copyright (c) 2012-2018, Intel Corporation

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
       this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
     * Neither the name of Intel Corporation nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AVX512
#include "intel-ipsec-mb.h"
#include "save_xmms.h"
#include "asm.h"
#include "des.h"

JOB_AES_HMAC *submit_job_aes128_enc_avx(MB_MGR_AES_OOO *state,
                                        JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_aes128_enc_avx(MB_MGR_AES_OOO *state);

JOB_AES_HMAC *submit_job_aes192_enc_avx(MB_MGR_AES_OOO *state,
                                        JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_aes192_enc_avx(MB_MGR_AES_OOO *state);

JOB_AES_HMAC *submit_job_aes256_enc_avx(MB_MGR_AES_OOO *state,
                                        JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_aes256_enc_avx(MB_MGR_AES_OOO *state);

JOB_AES_HMAC *submit_job_aes_xcbc_avx(MB_MGR_AES_XCBC_OOO *state,
                                      JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_aes_xcbc_avx(MB_MGR_AES_XCBC_OOO *state);

JOB_AES_HMAC *submit_job_des_cbc_enc_avx512(MB_MGR_DES_OOO *state,
                                            JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_des_cbc_enc_avx512(MB_MGR_DES_OOO *state);

JOB_AES_HMAC *submit_job_des_cbc_dec_avx512(MB_MGR_DES_OOO *state,
                                            JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_des_cbc_dec_avx512(MB_MGR_DES_OOO *state);

JOB_AES_HMAC *submit_job_3des_cbc_enc_avx512(MB_MGR_DES_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_3des_cbc_enc_avx512(MB_MGR_DES_OOO *state);

JOB_AES_HMAC *submit_job_3des_cbc_dec_avx512(MB_MGR_DES_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_3des_cbc_dec_avx512(MB_MGR_DES_OOO *state);

JOB_AES_HMAC *submit_job_docsis_des_enc_avx512(MB_MGR_DES_OOO *state,
                                               JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_docsis_des_enc_avx512(MB_MGR_DES_OOO *state);

JOB_AES_HMAC *submit_job_docsis_des_dec_avx512(MB_MGR_DES_OOO *state,
                                               JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_docsis_des_dec_avx512(MB_MGR_DES_OOO *state);

#define SAVE_XMMS save_xmms_avx
#define RESTORE_XMMS restore_xmms_avx
#define SUBMIT_JOB_AES128_ENC submit_job_aes128_enc_avx
#define SUBMIT_JOB_AES128_DEC submit_job_aes128_dec_avx
#define FLUSH_JOB_AES128_ENC  flush_job_aes128_enc_avx

#define SUBMIT_JOB_AES192_ENC submit_job_aes192_enc_avx
#define SUBMIT_JOB_AES192_DEC submit_job_aes192_dec_avx
#define FLUSH_JOB_AES192_ENC  flush_job_aes192_enc_avx

#define SUBMIT_JOB_AES256_ENC submit_job_aes256_enc_avx
#define SUBMIT_JOB_AES256_DEC submit_job_aes256_dec_avx
#define FLUSH_JOB_AES256_ENC  flush_job_aes256_enc_avx

#define SUBMIT_JOB_AES128_CNTR submit_job_aes128_cntr_avx
#define SUBMIT_JOB_AES192_CNTR submit_job_aes192_cntr_avx
#define SUBMIT_JOB_AES256_CNTR submit_job_aes256_cntr_avx


#define AES_CBC_DEC_128       aes_cbc_dec_128_avx
#define AES_CBC_DEC_192       aes_cbc_dec_192_avx
#define AES_CBC_DEC_256       aes_cbc_dec_256_avx

#define AES_CNTR_128       aes_cntr_128_avx
#define AES_CNTR_192       aes_cntr_192_avx
#define AES_CNTR_256       aes_cntr_256_avx

#define SUBMIT_JOB_AES_XCBC   submit_job_aes_xcbc_avx
#define FLUSH_JOB_AES_XCBC    flush_job_aes_xcbc_avx

#define SUBMIT_JOB_AES128_DEC submit_job_aes128_dec_avx
#define SUBMIT_JOB_AES192_DEC submit_job_aes192_dec_avx
#define SUBMIT_JOB_AES256_DEC submit_job_aes256_dec_avx

#define SUBMIT_JOB_DES_CBC_ENC submit_job_des_cbc_enc_avx512
#define FLUSH_JOB_DES_CBC_ENC  flush_job_des_cbc_enc_avx512

#define SUBMIT_JOB_DES_CBC_DEC submit_job_des_cbc_dec_avx512
#define FLUSH_JOB_DES_CBC_DEC flush_job_des_cbc_dec_avx512

#define SUBMIT_JOB_3DES_CBC_ENC submit_job_3des_cbc_enc_avx512
#define FLUSH_JOB_3DES_CBC_ENC  flush_job_3des_cbc_enc_avx512

#define SUBMIT_JOB_3DES_CBC_DEC submit_job_3des_cbc_dec_avx512
#define FLUSH_JOB_3DES_CBC_DEC flush_job_3des_cbc_dec_avx512

#define SUBMIT_JOB_DOCSIS_DES_ENC submit_job_docsis_des_enc_avx512
#define FLUSH_JOB_DOCSIS_DES_ENC  flush_job_docsis_des_enc_avx512

#define SUBMIT_JOB_DOCSIS_DES_DEC submit_job_docsis_des_dec_avx512
#define FLUSH_JOB_DOCSIS_DES_DEC flush_job_docsis_des_dec_avx512

#define SUBMIT_JOB_AES_ENC SUBMIT_JOB_AES_ENC_AVX512
#define FLUSH_JOB_AES_ENC  FLUSH_JOB_AES_ENC_AVX512
#define SUBMIT_JOB_AES_DEC SUBMIT_JOB_AES_DEC_AVX512

JOB_AES_HMAC *submit_job_hmac_avx512(MB_MGR_HMAC_SHA_1_OOO *state,
                                     JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_avx512(MB_MGR_HMAC_SHA_1_OOO *state);

JOB_AES_HMAC *submit_job_hmac_sha_224_avx512(MB_MGR_HMAC_SHA_256_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_sha_224_avx512(MB_MGR_HMAC_SHA_256_OOO *state);

JOB_AES_HMAC *submit_job_hmac_sha_256_avx512(MB_MGR_HMAC_SHA_256_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_sha_256_avx512(MB_MGR_HMAC_SHA_256_OOO *state);

JOB_AES_HMAC *submit_job_hmac_sha_384_avx512(MB_MGR_HMAC_SHA_512_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_sha_384_avx512(MB_MGR_HMAC_SHA_512_OOO *state);

JOB_AES_HMAC *submit_job_hmac_sha_512_avx512(MB_MGR_HMAC_SHA_512_OOO *state,
                                             JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_sha_512_avx512(MB_MGR_HMAC_SHA_512_OOO *state);

JOB_AES_HMAC *submit_job_hmac_md5_avx2(MB_MGR_HMAC_MD5_OOO *state,
                                       JOB_AES_HMAC *job);
JOB_AES_HMAC *flush_job_hmac_md5_avx2(MB_MGR_HMAC_MD5_OOO *state);

#define SUBMIT_JOB_HMAC               submit_job_hmac_avx512
#define FLUSH_JOB_HMAC                flush_job_hmac_avx512
#define SUBMIT_JOB_HMAC_SHA_224       submit_job_hmac_sha_224_avx512
#define FLUSH_JOB_HMAC_SHA_224        flush_job_hmac_sha_224_avx512
#define SUBMIT_JOB_HMAC_SHA_256       submit_job_hmac_sha_256_avx512
#define FLUSH_JOB_HMAC_SHA_256        flush_job_hmac_sha_256_avx512
#define SUBMIT_JOB_HMAC_SHA_384       submit_job_hmac_sha_384_avx512
#define FLUSH_JOB_HMAC_SHA_384        flush_job_hmac_sha_384_avx512
#define SUBMIT_JOB_HMAC_SHA_512       submit_job_hmac_sha_512_avx512
#define FLUSH_JOB_HMAC_SHA_512        flush_job_hmac_sha_512_avx512
#define SUBMIT_JOB_HMAC_MD5           submit_job_hmac_md5_avx2
#define FLUSH_JOB_HMAC_MD5            flush_job_hmac_md5_avx2

#ifndef NO_GCM
#define AES_GCM_DEC_128   aes_gcm_dec_128_avx_gen4
#define AES_GCM_ENC_128   aes_gcm_enc_128_avx_gen4
#define AES_GCM_DEC_192   aes_gcm_dec_192_avx_gen4
#define AES_GCM_ENC_192   aes_gcm_enc_192_avx_gen4
#define AES_GCM_DEC_256   aes_gcm_dec_256_avx_gen4
#define AES_GCM_ENC_256   aes_gcm_enc_256_avx_gen4
#endif /* NO_GCM */

/* ====================================================================== */

#define SUBMIT_JOB         submit_job_avx512
#define FLUSH_JOB          flush_job_avx512
#define QUEUE_SIZE         queue_size_avx512
#define SUBMIT_JOB_NOCHECK submit_job_nocheck_avx512

/* ====================================================================== */

#define SUBMIT_JOB_HASH    SUBMIT_JOB_HASH_AVX512
#define FLUSH_JOB_HASH     FLUSH_JOB_HASH_AVX512

/* ====================================================================== */

#define AES_CFB_128_ONE    aes_cfb_128_one_avx512

void aes128_cbc_mac_x8(AES_ARGS_x8 *args, uint64_t len);

#define AES128_CBC_MAC     aes128_cbc_mac_x8

#define FLUSH_JOB_AES_CCM_AUTH     flush_job_aes_ccm_auth_arch
#define SUBMIT_JOB_AES_CCM_AUTH    submit_job_aes_ccm_auth_arch
#define AES_CCM_MAX_JOBS 8

#define FLUSH_JOB_AES_CMAC_AUTH    flush_job_aes_cmac_auth_arch
#define SUBMIT_JOB_AES_CMAC_AUTH   submit_job_aes_cmac_auth_arch
#define AES_CMAC_MAX_JOBS 8

/* ====================================================================== */

void
init_mb_mgr_avx512(MB_MGR *state)
{
        unsigned int j;
        uint8_t *p;

        /* Init AES out-of-order fields */
        state->aes128_ooo.lens[0] = 0;
        state->aes128_ooo.lens[1] = 0;
        state->aes128_ooo.lens[2] = 0;
        state->aes128_ooo.lens[3] = 0;
        state->aes128_ooo.lens[4] = 0;
        state->aes128_ooo.lens[5] = 0;
        state->aes128_ooo.lens[6] = 0;
        state->aes128_ooo.lens[7] = 0;
        state->aes128_ooo.unused_lanes = 0xF76543210;
        state->aes128_ooo.job_in_lane[0] = NULL;
        state->aes128_ooo.job_in_lane[1] = NULL;
        state->aes128_ooo.job_in_lane[2] = NULL;
        state->aes128_ooo.job_in_lane[3] = NULL;
        state->aes128_ooo.job_in_lane[4] = NULL;
        state->aes128_ooo.job_in_lane[5] = NULL;
        state->aes128_ooo.job_in_lane[6] = NULL;
        state->aes128_ooo.job_in_lane[7] = NULL;

        state->aes192_ooo.lens[0] = 0;
        state->aes192_ooo.lens[1] = 0;
        state->aes192_ooo.lens[2] = 0;
        state->aes192_ooo.lens[3] = 0;
        state->aes192_ooo.lens[4] = 0;
        state->aes192_ooo.lens[5] = 0;
        state->aes192_ooo.lens[6] = 0;
        state->aes192_ooo.lens[7] = 0;
        state->aes192_ooo.unused_lanes = 0xF76543210;
        state->aes192_ooo.job_in_lane[0] = NULL;
        state->aes192_ooo.job_in_lane[1] = NULL;
        state->aes192_ooo.job_in_lane[2] = NULL;
        state->aes192_ooo.job_in_lane[3] = NULL;
        state->aes192_ooo.job_in_lane[4] = NULL;
        state->aes192_ooo.job_in_lane[5] = NULL;
        state->aes192_ooo.job_in_lane[6] = NULL;
        state->aes192_ooo.job_in_lane[7] = NULL;


        state->aes256_ooo.lens[0] = 0;
        state->aes256_ooo.lens[1] = 0;
        state->aes256_ooo.lens[2] = 0;
        state->aes256_ooo.lens[3] = 0;
        state->aes256_ooo.lens[4] = 0;
        state->aes256_ooo.lens[5] = 0;
        state->aes256_ooo.lens[6] = 0;
        state->aes256_ooo.lens[7] = 0;
        state->aes256_ooo.unused_lanes = 0xF76543210;
        state->aes256_ooo.job_in_lane[0] = NULL;
        state->aes256_ooo.job_in_lane[1] = NULL;
        state->aes256_ooo.job_in_lane[2] = NULL;
        state->aes256_ooo.job_in_lane[3] = NULL;
        state->aes256_ooo.job_in_lane[4] = NULL;
        state->aes256_ooo.job_in_lane[5] = NULL;
        state->aes256_ooo.job_in_lane[6] = NULL;
        state->aes256_ooo.job_in_lane[7] = NULL;

        /* DOCSIS SEC BPI (AES CBC + AES CFB for partial block)
         * uses same settings as AES128 CBC.
         */
        state->docsis_sec_ooo.lens[0] = 0;
        state->docsis_sec_ooo.lens[1] = 0;
        state->docsis_sec_ooo.lens[2] = 0;
        state->docsis_sec_ooo.lens[3] = 0;
        state->docsis_sec_ooo.lens[4] = 0;
        state->docsis_sec_ooo.lens[5] = 0;
        state->docsis_sec_ooo.lens[6] = 0;
        state->docsis_sec_ooo.lens[7] = 0;
        state->docsis_sec_ooo.unused_lanes = 0xF76543210;
        state->docsis_sec_ooo.job_in_lane[0] = NULL;
        state->docsis_sec_ooo.job_in_lane[1] = NULL;
        state->docsis_sec_ooo.job_in_lane[2] = NULL;
        state->docsis_sec_ooo.job_in_lane[3] = NULL;
        state->docsis_sec_ooo.job_in_lane[4] = NULL;
        state->docsis_sec_ooo.job_in_lane[5] = NULL;
        state->docsis_sec_ooo.job_in_lane[6] = NULL;
        state->docsis_sec_ooo.job_in_lane[7] = NULL;

        /* DES, 3DES and DOCSIS DES (DES CBC + DES CFB for partial block) */
        /* - separate DES OOO for encryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->des_enc_ooo.lens[j] = 0;
                state->des_enc_ooo.job_in_lane[j] = NULL;
        }
        state->des_enc_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->des_enc_ooo.num_lanes_inuse = 0;
        memset(&state->des_enc_ooo.args, 0, sizeof(state->des_enc_ooo.args));

        /* - separate DES OOO for decryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->des_dec_ooo.lens[j] = 0;
                state->des_dec_ooo.job_in_lane[j] = NULL;
        }
        state->des_dec_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->des_dec_ooo.num_lanes_inuse = 0;
        memset(&state->des_dec_ooo.args, 0, sizeof(state->des_dec_ooo.args));

        /* - separate 3DES OOO for encryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->des3_enc_ooo.lens[j] = 0;
                state->des3_enc_ooo.job_in_lane[j] = NULL;
        }
        state->des3_enc_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->des3_enc_ooo.num_lanes_inuse = 0;
        memset(&state->des3_enc_ooo.args, 0, sizeof(state->des3_enc_ooo.args));

        /* - separate 3DES OOO for decryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->des3_dec_ooo.lens[j] = 0;
                state->des3_dec_ooo.job_in_lane[j] = NULL;
        }
        state->des3_dec_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->des3_dec_ooo.num_lanes_inuse = 0;
        memset(&state->des3_dec_ooo.args, 0, sizeof(state->des3_dec_ooo.args));

        /* - separate DOCSIS DES OOO for encryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->docsis_des_enc_ooo.lens[j] = 0;
                state->docsis_des_enc_ooo.job_in_lane[j] = NULL;
        }
        state->docsis_des_enc_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->docsis_des_enc_ooo.num_lanes_inuse = 0;
        memset(&state->docsis_des_enc_ooo.args, 0,
               sizeof(state->docsis_des_enc_ooo.args));

        /* - separate DES OOO for decryption */
        for (j = 0; j < AVX512_NUM_DES_LANES; j++) {
                state->docsis_des_dec_ooo.lens[j] = 0;
                state->docsis_des_dec_ooo.job_in_lane[j] = NULL;
        }
        state->docsis_des_dec_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->docsis_des_dec_ooo.num_lanes_inuse = 0;
        memset(&state->docsis_des_dec_ooo.args, 0,
               sizeof(state->docsis_des_dec_ooo.args));

        /* Init HMAC/SHA1 out-of-order fields */
        state->hmac_sha_1_ooo.lens[0] = 0;
        state->hmac_sha_1_ooo.lens[1] = 0;
        state->hmac_sha_1_ooo.lens[2] = 0;
        state->hmac_sha_1_ooo.lens[3] = 0;
        state->hmac_sha_1_ooo.lens[4] = 0;
        state->hmac_sha_1_ooo.lens[5] = 0;
        state->hmac_sha_1_ooo.lens[6] = 0;
        state->hmac_sha_1_ooo.lens[7] = 0;
        state->hmac_sha_1_ooo.lens[8] = 0;
        state->hmac_sha_1_ooo.lens[9] = 0;
        state->hmac_sha_1_ooo.lens[10] = 0;
        state->hmac_sha_1_ooo.lens[11] = 0;
        state->hmac_sha_1_ooo.lens[12] = 0;
        state->hmac_sha_1_ooo.lens[13] = 0;
        state->hmac_sha_1_ooo.lens[14] = 0;
        state->hmac_sha_1_ooo.lens[15] = 0;
        state->hmac_sha_1_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->hmac_sha_1_ooo.num_lanes_inuse = 0;
        for (j = 0; j < AVX512_NUM_SHA1_LANES; j++) {
                state->hmac_sha_1_ooo.ldata[j].job_in_lane = NULL;
                state->hmac_sha_1_ooo.ldata[j].extra_block[64] = 0x80;
                memset(state->hmac_sha_1_ooo.ldata[j].extra_block + 65,
                       0x00,
                       64 + 7);
                p = state->hmac_sha_1_ooo.ldata[j].outer_block;
                memset(p + 5*4 + 1,
                       0x00,
                       64 - 5*4 - 1 - 2);
                p[5 * 4] = 0x80;
                p[64 - 2] = 0x02;
                p[64 - 1] = 0xA0;
        }

        /* Init HMAC/SHA224 out-of-order fields */
        state->hmac_sha_224_ooo.lens[0] = 0;
        state->hmac_sha_224_ooo.lens[1] = 0;
        state->hmac_sha_224_ooo.lens[2] = 0;
        state->hmac_sha_224_ooo.lens[3] = 0;
        state->hmac_sha_224_ooo.lens[4] = 0;
        state->hmac_sha_224_ooo.lens[5] = 0;
        state->hmac_sha_224_ooo.lens[6] = 0;
        state->hmac_sha_224_ooo.lens[7] = 0;
        state->hmac_sha_224_ooo.lens[8] = 0;
        state->hmac_sha_224_ooo.lens[9] = 0;
        state->hmac_sha_224_ooo.lens[10] = 0;
        state->hmac_sha_224_ooo.lens[11] = 0;
        state->hmac_sha_224_ooo.lens[12] = 0;
        state->hmac_sha_224_ooo.lens[13] = 0;
        state->hmac_sha_224_ooo.lens[14] = 0;
        state->hmac_sha_224_ooo.lens[15] = 0;
        state->hmac_sha_224_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->hmac_sha_224_ooo.num_lanes_inuse = 0;
        /* sha256 and sha224 are very similar except for
         * digest constants and output size
         */
        for (j = 0; j < AVX512_NUM_SHA256_LANES; j++) {
                state->hmac_sha_224_ooo.ldata[j].job_in_lane = NULL;
                state->hmac_sha_224_ooo.ldata[j].extra_block[64] = 0x80;
                memset(state->hmac_sha_224_ooo.ldata[j].extra_block + 65,
                       0x00,
                       64 + 7);
                p = state->hmac_sha_224_ooo.ldata[j].outer_block;
                memset(p + 8*4 + 1,
                       0x00,
                       64 - 8*4 - 1 - 2);
                p[7 * 4] = 0x80;  /* digest 7 words long */
                p[64 - 2] = 0x02; /* length in little endian = 0x02E0 */
                p[64 - 1] = 0xE0;
        }

        /* Init HMAC/SHA256 out-of-order fields */
        state->hmac_sha_256_ooo.lens[0] = 0;
        state->hmac_sha_256_ooo.lens[1] = 0;
        state->hmac_sha_256_ooo.lens[2] = 0;
        state->hmac_sha_256_ooo.lens[3] = 0;
        state->hmac_sha_256_ooo.lens[4] = 0;
        state->hmac_sha_256_ooo.lens[5] = 0;
        state->hmac_sha_256_ooo.lens[6] = 0;
        state->hmac_sha_256_ooo.lens[7] = 0;
        state->hmac_sha_256_ooo.lens[8] = 0;
        state->hmac_sha_256_ooo.lens[9] = 0;
        state->hmac_sha_256_ooo.lens[10] = 0;
        state->hmac_sha_256_ooo.lens[11] = 0;
        state->hmac_sha_256_ooo.lens[12] = 0;
        state->hmac_sha_256_ooo.lens[13] = 0;
        state->hmac_sha_256_ooo.lens[14] = 0;
        state->hmac_sha_256_ooo.lens[15] = 0;
        state->hmac_sha_256_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->hmac_sha_256_ooo.num_lanes_inuse = 0;
        for (j = 0; j < AVX512_NUM_SHA256_LANES; j++) {
                state->hmac_sha_256_ooo.ldata[j].job_in_lane = NULL;
                state->hmac_sha_256_ooo.ldata[j].extra_block[64] = 0x80;
                memset(state->hmac_sha_256_ooo.ldata[j].extra_block + 65,
                       0x00,
                       64 + 7);
                /* hmac related */
                p = state->hmac_sha_256_ooo.ldata[j].outer_block;
                memset(p + 8*4 + 1,
                       0x00,
                       64 - 8*4 - 1 - 2);
                p[8 * 4] = 0x80;  /* 8 digest words */
                p[64 - 2] = 0x03; /* length */
                p[64 - 1] = 0x00;
        }

        /* Init HMAC/SHA384 out-of-order fields */
        state->hmac_sha_384_ooo.lens[0] = 0;
        state->hmac_sha_384_ooo.lens[1] = 0;
        state->hmac_sha_384_ooo.lens[2] = 0;
        state->hmac_sha_384_ooo.lens[3] = 0;
        state->hmac_sha_384_ooo.lens[4] = 0;
        state->hmac_sha_384_ooo.lens[5] = 0;
        state->hmac_sha_384_ooo.lens[6] = 0;
        state->hmac_sha_384_ooo.lens[7] = 0;
        state->hmac_sha_384_ooo.unused_lanes = 0xF76543210;
        for (j = 0; j < AVX512_NUM_SHA512_LANES; j++) {
                MB_MGR_HMAC_SHA_512_OOO *ctx = &state->hmac_sha_384_ooo;

                ctx->ldata[j].job_in_lane = NULL;
                ctx->ldata[j].extra_block[SHA_384_BLOCK_SIZE] = 0x80;
                memset(ctx->ldata[j].extra_block + (SHA_384_BLOCK_SIZE + 1),
                       0x00, SHA_384_BLOCK_SIZE + 7);
                p = ctx->ldata[j].outer_block;
                /* special end point because this length is constant */
                memset(p + SHA384_DIGEST_SIZE_IN_BYTES  + 1, 0x00,
                       SHA_384_BLOCK_SIZE -
                       SHA384_DIGEST_SIZE_IN_BYTES  - 1 - 2);
                /* mark the end */
                p[SHA384_DIGEST_SIZE_IN_BYTES] = 0x80;
                /* hmac outer block length always of fixed size,
                 * it is OKey length, a whole message block length, 1024 bits,
                 * with padding plus the length of the inner digest,
                 * which is 384 bits, 1408 bits == 0x0580.
                 * The input message block needs to be converted to big endian
                 * within the sha implementation before use.
                 */
                p[SHA_384_BLOCK_SIZE - 2] = 0x05;
                p[SHA_384_BLOCK_SIZE - 1] = 0x80;
        }

        /* Init HMAC/SHA512 out-of-order fields */
        state->hmac_sha_512_ooo.lens[0] = 0;
        state->hmac_sha_512_ooo.lens[1] = 0;
        state->hmac_sha_512_ooo.lens[2] = 0;
        state->hmac_sha_512_ooo.lens[3] = 0;
        state->hmac_sha_512_ooo.lens[4] = 0;
        state->hmac_sha_512_ooo.lens[5] = 0;
        state->hmac_sha_512_ooo.lens[6] = 0;
        state->hmac_sha_512_ooo.lens[7] = 0;
        state->hmac_sha_512_ooo.unused_lanes = 0xF76543210;
        for (j = 0; j < AVX512_NUM_SHA512_LANES; j++) {
                MB_MGR_HMAC_SHA_512_OOO *ctx = &state->hmac_sha_512_ooo;

                ctx->ldata[j].job_in_lane = NULL;
                ctx->ldata[j].extra_block[SHA_512_BLOCK_SIZE] = 0x80;
                memset(ctx->ldata[j].extra_block + (SHA_512_BLOCK_SIZE + 1),
                       0x00, SHA_512_BLOCK_SIZE + 7);
                p = ctx->ldata[j].outer_block;
                /* special end point because this length is constant */
                memset(p + SHA512_DIGEST_SIZE_IN_BYTES  + 1, 0x00,
                       SHA_512_BLOCK_SIZE -
                       SHA512_DIGEST_SIZE_IN_BYTES  - 1 - 2);
                /* mark the end */
                p[SHA512_DIGEST_SIZE_IN_BYTES] = 0x80;
                /* hmac outer block length always of fixed size,
                 * it is OKey length, a whole message block length, 1024 bits,
                 * with padding plus the length of the inner digest,
                 * which is 512 bits, 1536 bits == 0x600.
                 * The input message block needs to be converted to big endian
                 * within the sha implementation before use.
                 */
                p[SHA_512_BLOCK_SIZE - 2] = 0x06;
                p[SHA_512_BLOCK_SIZE - 1] = 0x00;
        }

        /* Init HMAC/MD5 out-of-order fields */
        state->hmac_md5_ooo.lens[0] = 0;
        state->hmac_md5_ooo.lens[1] = 0;
        state->hmac_md5_ooo.lens[2] = 0;
        state->hmac_md5_ooo.lens[3] = 0;
        state->hmac_md5_ooo.lens[4] = 0;
        state->hmac_md5_ooo.lens[5] = 0;
        state->hmac_md5_ooo.lens[6] = 0;
        state->hmac_md5_ooo.lens[7] = 0;
        state->hmac_md5_ooo.lens[8] = 0;
        state->hmac_md5_ooo.lens[9] = 0;
        state->hmac_md5_ooo.lens[10] = 0;
        state->hmac_md5_ooo.lens[11] = 0;
        state->hmac_md5_ooo.lens[12] = 0;
        state->hmac_md5_ooo.lens[13] = 0;
        state->hmac_md5_ooo.lens[14] = 0;
        state->hmac_md5_ooo.lens[15] = 0;
        state->hmac_md5_ooo.unused_lanes = 0xFEDCBA9876543210;
        state->hmac_md5_ooo.num_lanes_inuse = 0;
        for (j = 0; j < AVX512_NUM_MD5_LANES; j++) {
                state->hmac_md5_ooo.ldata[j].job_in_lane = NULL;
                state->hmac_md5_ooo.ldata[j].extra_block[64] = 0x80;
                memset(state->hmac_md5_ooo.ldata[j].extra_block + 65,
                       0x00,
                       64 + 7);
                p = state->hmac_md5_ooo.ldata[j].outer_block;
                memset(p + 5*4 + 1,
                       0x00,
                       64 - 5*4 - 1 - 2);
                p[4 * 4] = 0x80;
                p[64 - 7] = 0x02;
                p[64 - 8] = 0x80;
        }

        /* Init AES/XCBC OOO fields */
        state->aes_xcbc_ooo.lens[0] = 0;
        state->aes_xcbc_ooo.lens[1] = 0;
        state->aes_xcbc_ooo.lens[2] = 0;
        state->aes_xcbc_ooo.lens[3] = 0;
        state->aes_xcbc_ooo.lens[4] = 0;
        state->aes_xcbc_ooo.lens[5] = 0;
        state->aes_xcbc_ooo.lens[6] = 0;
        state->aes_xcbc_ooo.lens[7] = 0;
        state->aes_xcbc_ooo.unused_lanes = 0xF76543210;
        for (j = 0; j < 8 ; j++) {
                state->aes_xcbc_ooo.ldata[j].job_in_lane = NULL;
                state->aes_xcbc_ooo.ldata[j].final_block[16] = 0x80;
                memset(state->aes_xcbc_ooo.ldata[j].final_block + 17, 0x00, 15);
        }

        /* Init AES-CCM auth out-of-order fields */
        for (j = 0; j < 8; j++) {
                state->aes_ccm_ooo.init_done[j] = 0;
                state->aes_ccm_ooo.lens[j] = 0;
                state->aes_ccm_ooo.job_in_lane[j] = NULL;
        }
        state->aes_ccm_ooo.unused_lanes = 0xF76543210;

        /* Init AES-CMAC auth out-of-order fields */
        for (j = 0; j < 8; j++) {
                state->aes_cmac_ooo.init_done[j] = 0;
                state->aes_cmac_ooo.lens[j] = 0;
                state->aes_cmac_ooo.job_in_lane[j] = NULL;
        }
        state->aes_cmac_ooo.unused_lanes = 0xF76543210;

        /* Init "in order" components */
        state->next_job = 0;
        state->earliest_job = -1;

        /* set handlers */
        state->get_next_job        = get_next_job_avx512;
        state->submit_job          = submit_job_avx512;
        state->submit_job_nocheck  = submit_job_nocheck_avx512;
        state->get_completed_job   = get_completed_job_avx512;
        state->flush_job           = flush_job_avx512;
        state->queue_size          = queue_size_avx512;
        state->keyexp_128          = aes_keyexp_128_avx512;
        state->keyexp_192          = aes_keyexp_192_avx512;
        state->keyexp_256          = aes_keyexp_256_avx512;
        state->cmac_subkey_gen_128 = aes_cmac_subkey_gen_avx512;
        state->xcbc_keyexp         = aes_xcbc_expand_key_avx512;
        state->des_key_sched       = des_key_schedule;
        state->sha1_one_block      = sha1_one_block_avx512;
        state->sha224_one_block    = sha224_one_block_avx512;
        state->sha256_one_block    = sha256_one_block_avx512;
        state->sha384_one_block    = sha384_one_block_avx512;
        state->sha512_one_block    = sha512_one_block_avx512;
        state->md5_one_block       = md5_one_block_avx512;
}

#include "mb_mgr_code.h"
