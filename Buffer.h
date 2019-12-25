//
// Created by zhangjianxiong on 2019/12/12.
//

#ifndef HORIZON_BUFFER_H
#define HORIZON_BUFFER_H

#include <string>

#include "Socket.h"
namespace Horizon
{
	constexpr int kRead_iovec_num = 4;
	constexpr int kBuffer_size_min = 1024;
	constexpr size_t kBuffer_chain_max = UINT64_MAX;
    struct buffer_chain
    {
        buffer_chain* next;
        size_t buffer_len;
        size_t misalign;
        size_t off;
        unsigned char* buffer;
        buffer_chain()
        :next(nullptr),buffer_len(0),misalign(0),off(0),buffer(nullptr){}
    };
    constexpr size_t kBuffer_chain_size = sizeof(buffer_chain);

    class Buffer
    {
    public:
        Buffer();
        ~Buffer();
        //user add data to buffer
        bool Add_data_to_buffer(const char* data_in, size_t datalen);
        //bool Add_data_to_buffer(const std::string& data);
        //user extract data from buffer
        size_t Extract_data_from_buffer(char* data_out,size_t datalen);
        //scatter read
        //if want to read all data in fd ,set howmuch<0;
        size_t Read_data_from_fd(const Socket& sock);
        //gather write
        //if want to write all in buffer to fd,set howmuch<0;
        size_t Write_data_to_fd(const Socket& sock);
        const size_t& data_size(){ return _total_len;}
        bool empty(){ return _total_len==0;}
    private:
        //create new buffer_chain
        buffer_chain* Create_buffer_chain(size_t size);
        //detory new buffer
        void Destory_buffer();
        //insert new buffer_chain
        void Insert_buffer_chain(buffer_chain *chain);
        //remove data and reuse buffer_chain
        void Remove_data_and_reuse_chain(size_t len);
        // should realign buffer_chain?
        bool Is_chain_realign(buffer_chain* chain, size_t datalen);
        //realign buffer_chain and make align=0;
        void Align_buffer_chain(buffer_chain* chain);
        //expand buffer to contend all data
        void Expand_buffer(size_t datalen, int n);
        int Set_iovec_attr(size_t howmuch, struct iovec* vecs,int vec_size,buffer_chain*& pChain);
    private:
        buffer_chain* _first;
        buffer_chain* _last;
        buffer_chain* _last_with_datap;
        size_t _total_len;
        size_t _chain_size;
    };
}
#endif //HORIZON_BUFFER_H
