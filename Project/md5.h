#pragma once

const unsigned int MD5_SIZE = (4 * sizeof(unsigned int));   /* 16 */
const unsigned int MD5_STRING_SIZE = 2 * MD5_SIZE + 1;      /* 33 */

 namespace md5 {
    const unsigned int BLOCK_SIZE = 64;

    class md5_t {
        public:
            md5_t();
            md5_t(const void* input, const unsigned int input_length, void* signature_ = NULL);
            void process(const void* input, const unsigned int input_length);
            void finish(void* signature_ = NULL);
            void get_sig(void* signature_);
            void get_string(void* str_);

        private:
            /* internal functions */
            void initialise();
            void process_block(const unsigned char*);
            void get_result(void*);

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

    extern void sig_to_string(const void* signature, char* str, const int str_len);
    extern void sig_from_string(void* signature, const char* str);
}
