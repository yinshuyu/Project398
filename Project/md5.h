#pragma once



#include "cuda_runtime.h" //cudaDeviceProp



const unsigned int MD5_SIZE = (4 * sizeof(unsigned int));   /* 16 */
const unsigned int MD5_STRING_SIZE = 2 * MD5_SIZE + 1;      /* 33 */

 namespace md5 {
    const unsigned int BLOCK_SIZE = 64;

    class md5_t {
        public:
            __host__ __device__ md5_t();
            __host__ __device__ md5_t(const void* input, const unsigned int input_length, void* signature_ = NULL);
            __host__ __device__ void process(const void* input, const unsigned int input_length);
            __host__ __device__ void finish(void* signature_ = NULL);
            __host__ __device__ void get_sig(void* signature_);
            __host__ __device__ void get_string(void* str_);

        private:
            /* internal functions */
            __host__ __device__ void initialise();
            __host__ __device__ void process_block(const unsigned char*);
            __host__ __device__ void get_result(void*);

            unsigned int A;                             /* accumulator 1 */
            unsigned int B;                             /* accumulator 2 */
            unsigned int C;                             /* accumulator 3 */
            unsigned int D;                             /* accumulator 4 */

            unsigned int message_length[2];             /* length of data */
            unsigned int stored_size;                   /* length of stored bytes */
            unsigned char stored[md5::BLOCK_SIZE * 2];  /* stored bytes */

            bool finished;                              /* object state */

            char signature[MD5_SIZE];                   /* stored signature */
            char str[MD5_STRING_SIZE];                  /* stored plain text hash */
    };

    __host__ __device__ extern "C"  void sig_to_string(const void* signature, char* str, const int str_len);
    __host__ __device__ extern "C"  void sig_from_string(void* signature, const char* str);
}
