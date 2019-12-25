//
// Created by zhangjianxiong on 2019/12/12.
//
#include <stdexcept>
#include <cassert>
#include <cstring>
#include <sys/ioctl.h>
#include <sys/uio.h>

#include "Buffer.h"
#include "Log.h"
namespace Horizon
{
    Buffer::Buffer()
    :_first(nullptr),
     _last(nullptr),
     _total_len(0),
     _chain_size(0)
    {
        _last_with_datap = _first;
    }
    Buffer::~Buffer()
    {
        Destory_buffer();
    }
    bool Buffer::Add_data_to_buffer(const char* data_in, size_t datalen)
    {
        buffer_chain* chain = nullptr, *temp = nullptr;
        size_t remain = 0;
        size_t toalloc = 0;
        if (datalen > UINT64_MAX-_total_len)
        {
            LOG_ERR << "memory is not enough";
            throw(std::overflow_error("memory is not enough"));
        }
        if (_last_with_datap==nullptr)
        {
            chain = _last;
        }
        else
        {
            chain = _last_with_datap;
        }
        if (chain == nullptr)
        {
            try
            {
                chain = Create_buffer_chain(datalen);
            }
            catch (...)
            {
                LOG_ERR << "crate buffer_chain failed";
                throw;
            }
            Insert_buffer_chain(chain);
        }
        remain = chain->buffer_len - (size_t)chain->misalign - chain->off;
        if (remain>=datalen)
        {
            //剩余空间足够，或者新建了一个chain
            ::memcpy(chain->buffer + chain->misalign + chain->off, data_in, datalen);
            chain->off += datalen;
            _total_len += datalen;
            return true;
        }
        else if(Is_chain_realign(chain, datalen))
        {
            Align_buffer_chain(chain);
            ::memcpy(chain->buffer + chain->off, data_in, datalen);
            chain->off += datalen;
            _total_len += datalen;
            return true;
        }
        else
        {
            //原有的chain有剩余空间，但剩余量不够
            ::memcpy(chain->buffer + chain->misalign + chain->off, data_in, remain);
            chain->off += remain;
            _total_len += remain;
            datalen -= remain;
            data_in += remain;
            //将剩余数据拷入新的chain中
            toalloc = chain->buffer_len;
            if (toalloc<datalen)
            {
                toalloc = datalen;
            }
            try
            {
                temp = Create_buffer_chain(toalloc);
            }
            catch (...)
            {
                LOG_ERR << "crate buffer_chain failed";
                throw;
            }
            Insert_buffer_chain(temp);
            ::memcpy(temp->buffer, data_in, datalen);
            temp->off = datalen;
            _total_len += datalen;
            return true;
        }

    }
    /*
    bool Buffer::Add_data_to_buffer(const std::string& data)
    {
        auto data_in=data.c_str();
        auto data_len=data.length();
        return Add_data_to_buffer(data_in,data_len);
    }*/
    size_t Buffer::Extract_data_from_buffer(char* data_out,size_t datalen)
    {
        if(_total_len==0)
        {
            return 0;
        }
        if(datalen == 0)
		{
          datalen=_total_len;
		}
        buffer_chain* chain= _first;
        char* data=data_out;
        size_t nread=0;
        if(data_out == nullptr || datalen<=0)
        {
            LOG_WARN << "invalid input argments";
            return -1;
        }
        if(nread > _total_len)
		{
        	nread = _total_len;
		}else
		{
			nread=datalen;
		}
        while(datalen && datalen >= chain->off)
        {
            ::memcpy(data,chain->buffer+chain->misalign,chain->off);
            data+=chain->off;
            datalen-=chain->off;
            chain=chain->next;
            if(chain == _last_with_datap && datalen>chain->off)
            {
                datalen=chain->off;
            }
        }
        if(datalen)
        {
            assert(chain);
            assert(datalen<chain->off);
            ::memcpy(data,chain->buffer+chain->misalign,datalen);
        }
        Remove_data_and_reuse_chain(nread);
        return nread;
    }

    size_t Buffer::Read_data_from_fd(const Socket& sock)
    {
        buffer_chain* pChain= nullptr;
        int n=0,nvecs=0,i=0;
        size_t howmuch=0;
        int fd = sock.fd();
        if(::ioctl(fd,FIONREAD,&howmuch)<0)
        {
            LOG_ERR << "ioctl error";
            throw (std::length_error("data size must be positive"));
        }
        try
        {
            Expand_buffer(howmuch, kRead_iovec_num);
        }
        catch (...)
        {
            LOG_ERR << "expand buffer failed";
            throw;
        }
        struct iovec vec[kRead_iovec_num];
        nvecs = Set_iovec_attr(howmuch,vec,kRead_iovec_num,pChain);
        n=::readv(fd,vec,nvecs);
        if(n==-1)
        {
            return -1;
        }
        if(n==0)
        {
            return 0;
        }
        for(i=0;i<nvecs;i++)
        {
            pChain->off+=vec[i].iov_len;
            pChain=pChain->next;
        }
        pChain=_first;
        while(pChain->next != nullptr && pChain->next->off != 0)
		{
        	pChain=pChain->next;
		}
        _last_with_datap=pChain;
        _total_len+=n;
        return n;
    }
    size_t Buffer::Write_data_to_fd(const Socket& sock)
    {
        size_t howmuch = _total_len;
        int i=0;
        size_t n=0;
        int fd = sock.fd();
        buffer_chain* chain = _first;
        struct iovec vec[_chain_size];
        while(howmuch>0)
        {
            vec[i].iov_base=(void*)(chain+chain->misalign);
            if(howmuch > chain->off)
            {
                vec[i].iov_len=chain->off;
                howmuch-=chain->off;
            }
            else {
                vec[i].iov_len = howmuch;
                howmuch = 0;
            }
        }
        n = ::writev(fd,vec,i);
        if(n<0)
        {
            LOG_ERR << "writev error,"<< "error number is" <<errno;
            if(errno == EWOULDBLOCK || errno == EINTR)
            {
                return errno;
            }
        }
        Remove_data_and_reuse_chain(howmuch);
        return n;
    }

    int Buffer::Set_iovec_attr(size_t howmuch, struct iovec* vecs,int vec_size,buffer_chain*& pChain)
    {
        buffer_chain* chain = nullptr;
        buffer_chain* firstAvailable= nullptr;
        size_t all_avail=0;
        int i=0;
        firstAvailable=_last_with_datap;
        assert(firstAvailable != nullptr);
        if(firstAvailable->buffer_len-firstAvailable->misalign-firstAvailable->off==0)
        {
            firstAvailable=firstAvailable->next;
        }
        chain=firstAvailable;
        for (i = 0; i < vec_size && all_avail < howmuch; ++i) {
            size_t avail=chain->buffer_len-chain->misalign-chain->off;
            if(avail > howmuch-all_avail)
            {
                avail=howmuch-all_avail;
                all_avail=howmuch;
            }
            vecs[i].iov_base=(void*)(chain->buffer+chain->misalign+chain->off);
            vecs[i].iov_len=avail;
        }
        pChain=firstAvailable;
        return i;
    }
    void Buffer::Expand_buffer(size_t datalen, int n)
    {
        buffer_chain *chain = nullptr, *tmp = nullptr;
        size_t avail = 0;
        int used = 0;
        assert(n >= 2);
        for (chain=_last_with_datap;chain!=nullptr;chain=chain->next)
        {
            if (chain->off>0)
            {
                size_t space = chain->buffer_len - chain->misalign - chain->off;
                if (space>0)
                {
                    avail += space;
                    used++;
                }
            }
            else
            {
                chain->misalign = 0;
                avail += chain->buffer_len;
                used++;
            }
            if (avail>=datalen)
            {
                return;
            }
            if (used==n)
            {
                break;
            }
        }
        if (used < n)
        {
            assert(chain == nullptr);
            datalen -= avail;
            try
            {
                tmp = Create_buffer_chain(datalen);
            }
            catch (...)
            {
                LOG_ERR << "Create_buffer_chain failed";
                throw;
            }
            Insert_buffer_chain(tmp);
            return;
        }
        else
        {
            chain = _last_with_datap;
            avail = chain->buffer_len - chain->misalign - chain->off;
            datalen -= avail;
            try
            {
                tmp = Create_buffer_chain(datalen);
            }
            catch (...)
            {
                LOG_ERR << "Create_buffer_chain failed";
                throw;
            }
            tmp->next=_last_with_datap->next;
            _last_with_datap->next = tmp;
            return;
        }
    }
    buffer_chain* Buffer::Create_buffer_chain(size_t size)
    {
        size_t alloc = 0;
        buffer_chain* chain = nullptr;
        void* mm_ptr= nullptr;
        if (size > kBuffer_chain_max - kBuffer_chain_size)
        {
            LOG_ERR << "SIZE IS TOO LARGE";
            throw(std::length_error("size is too large"));
        }
        size += kBuffer_chain_size;
        if (size < kBuffer_chain_max/2)
        {
            alloc = kBuffer_size_min;
            while (alloc < size)
            {
                alloc <<= 1;
            }
        }
        else
        {
            alloc = size;
        }
        if ((mm_ptr =::operator new(alloc))==nullptr)
        {
            LOG_ERR << "memory malloc failed";
            throw(std::logic_error("memory malloc failed"));
        }
        chain=::new(mm_ptr)buffer_chain();
        chain->buffer_len = alloc - kBuffer_chain_size;
        chain->buffer = reinterpret_cast<unsigned char*>(chain + 1);
        return chain;
    }
    void Buffer::Destory_buffer()
    {
        buffer_chain* chain = _first;
        buffer_chain* next = nullptr;
        while(chain != nullptr)
        {
            next=chain->next;
            ::delete chain;
            chain = next;
        }
    }
    void Buffer::Remove_data_and_reuse_chain(size_t len)
    {
        buffer_chain* chain,*next,*first;
        size_t remaining,old_len;
        old_len=_total_len;
        if(old_len == 0)
            return;
		assert(_first);
        if(len >= old_len)
        {
        	chain =_first;
        	while(chain != _last_with_datap->next)
			{
        		chain->misalign=0;
        		chain->off = 0;
        		chain = chain->next;
			}
            _last_with_datap=_first;
            _total_len=0;
        }
        else
        {
            _total_len-=len;
            remaining=len;
            first = _first;
            for(chain = first;remaining>=chain->off;chain=next)
            {
                next=chain->next;
                remaining-=chain->off;
                chain->misalign=0;
                chain->off=0;
                if(next != _last_with_datap->next)
				{
					_first = next;
					_last->next=chain;
					_last=chain;
				}
            }
            assert(remaining < chain->off);
            chain->misalign+=remaining;
            chain->off -= remaining;
        }
    }
    void Buffer::Insert_buffer_chain(buffer_chain *chain)
    {
        if (_last_with_datap == nullptr)
        {
            assert(_first == nullptr);
            _first = _last = chain;
            _last_with_datap = chain;
        }
        else
        {
            buffer_chain* pChain = _last_with_datap->next;
            chain->next=pChain;
            _last_with_datap->next = chain;
            if (chain->off)
            {
                _last_with_datap = chain;
            }
        }
        _total_len += chain->off;
        _chain_size++;
    }
    bool Buffer::Is_chain_realign(buffer_chain* chain, size_t datalen)
    {
        return (chain->buffer_len - chain->off >= datalen &&
                (chain->off < chain->buffer_len / 2) &&
                (chain->off <= 2048));
    }
    void Buffer::Align_buffer_chain(buffer_chain* chain)
    {
        ::memmove(chain->buffer, chain->buffer + chain->misalign, chain->off);
        chain->misalign = 0;
    }
}
