/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2014 QuantumCore
 *
 * Copyright (C) 2009-2014 Crispi Custom Scripts Special For TrinityCore
 *
 * Copyright (C) 2013-2014 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef QUANTUM_DATA_PACKETS_H
#define QUANTUM_DATA_PACKETS_H

#include <list>

extern uint32 urand(uint32 min, uint32 max);

namespace Quantum
{
    namespace DataPackets
    {
        template<class T>
        void RandomResizeList(std::list<T> &list, uint32 size)
        {
            size_t list_size = list.size();

            while (list_size > size)
            {
                typename std::list<T>::iterator itr = list.begin();
                std::advance(itr, urand(0, list_size - 1));
                list.erase(itr);
                --list_size;
            }
        }

        template<class T, class Predicate>
        void RandomResizeList(std::list<T> &list, Predicate& predicate, uint32 size)
        {
            std::list<T> listCopy;
            for (typename std::list<T>::iterator itr = list.begin(); itr != list.end(); ++itr)
                if (predicate(*itr))
                    listCopy.push_back(*itr);

            if (size)
                RandomResizeList(listCopy, size);

            list = listCopy;
        }

        template <class C> typename C::value_type const& SelectRandomContainerElement(C const& container)
        {
            typename C::const_iterator it = container.begin();
            std::advance(it, urand(0, container.size() - 1));
            return *it;
        }
    };
};

#endif