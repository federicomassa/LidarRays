/*
 * Modified version of portable binary defined in cereal
 * https://github.com/USCiLab/cereal/blob/v1.2.2/include/cereal/archives/portable_binary.hpp
 */
#pragma once

#include <cereal/cereal.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <sstream>
#include <limits>

namespace simulink {

    class SimulinkOutputArchive : public cereal::OutputArchive<SimulinkOutputArchive, cereal::AllowEmptyClassElision> {
        std::ostream& _stream;
        const bool _convert_endianness;

    public:
        SimulinkOutputArchive(std::ostream& stream) :
            cereal::OutputArchive<SimulinkOutputArchive, cereal::AllowEmptyClassElision>(this),
            _stream(stream),
            _convert_endianness(cereal::portable_binary_detail::is_little_endian()) {}

        template<std::size_t DataSize>
        void save_binary(const void* data, std::size_t size){
            std::size_t written_size = 0;

            if(_convert_endianness){
                for(std::size_t i = 0; i < size; i += DataSize)
                    for(std::size_t j = 0; j < DataSize; ++j)
                        written_size += static_cast<std::size_t>(_stream.rdbuf()->sputn(reinterpret_cast<const char*>(data) + DataSize - j - 1 + i, 1));
            }
            else
                written_size = static_cast<std::size_t>(_stream.rdbuf()->sputn(reinterpret_cast<const char*>(data), size));

            if(written_size != size)
                throw cereal::Exception("Failed to write " + std::to_string(size) + " bytes to output stream! Wrote " + std::to_string(written_size));
        }
    };

    class SimulinkInputArchive : public cereal::InputArchive<SimulinkInputArchive, cereal::AllowEmptyClassElision>{
        std::istream& _stream;
        const uint8_t _convert_endianness;
    public:
        SimulinkInputArchive(std::istream& stream) :
            cereal::InputArchive<SimulinkInputArchive, cereal::AllowEmptyClassElision>(this),
            _stream(stream),
            _convert_endianness(cereal::portable_binary_detail::is_little_endian()){}

        template <std::size_t DataSize>
        void load_binary(void* const data, std::size_t size){
            auto const read_size = static_cast<std::size_t>(_stream.rdbuf()->sgetn(reinterpret_cast<char*>(data), size));

            if(read_size != size)
            throw cereal::Exception("Failed to read " + std::to_string(size) + " bytes from input stream! Read " + std::to_string(read_size));

            // flip bits if needed
            if(_convert_endianness){
                std::uint8_t* ptr = reinterpret_cast<std::uint8_t*>(data);
                for(std::size_t i = 0; i < size; i += DataSize)
                    cereal::portable_binary_detail::swap_bytes<DataSize>(ptr + i);
            }
        }      
    };

    template<class T>
    inline
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    CEREAL_SAVE_FUNCTION_NAME(SimulinkOutputArchive& ar, const T & t){
        static_assert(!std::is_floating_point<T>::value ||
                    (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                    "Portable binary only supports IEEE 754 stand ardized floating point");
        ar.save_binary<sizeof(T)>(std::addressof(t), sizeof(t));
    }

    template<class T>
    inline
    typename std::enable_if<std::is_arithmetic<T>::value, void>::type
    CEREAL_LOAD_FUNCTION_NAME(SimulinkInputArchive& ar, T& t){
        static_assert(!std::is_floating_point<T>::value ||
                    (std::is_floating_point<T>::value && std::numeric_limits<T>::is_iec559),
                    "Portable binary only supports IEEE 754 standardized floating point" );
        ar.load_binary<sizeof(T)>(std::addressof(t), sizeof(t));
    }

    template <class Archive, class T>
    inline
    CEREAL_ARCHIVE_RESTRICT(SimulinkInputArchive, SimulinkOutputArchive)
    CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cereal::NameValuePair<T>& t){
        ar(t.value);
    }

    template <class Archive, class T>
    inline
    CEREAL_ARCHIVE_RESTRICT(SimulinkInputArchive, SimulinkOutputArchive)
    CEREAL_SERIALIZE_FUNCTION_NAME(Archive& ar, cereal::SizeTag<T>& t){
        ar(t.size);
    }

    template <class T>
    inline
    void CEREAL_SAVE_FUNCTION_NAME(SimulinkOutputArchive& ar, const cereal::BinaryData<T>& bd){
        typedef typename std::remove_pointer<T>::type TT;
        static_assert(!std::is_floating_point<TT>::value ||
                    (std::is_floating_point<TT>::value && std::numeric_limits<TT>::is_iec559),
                    "Portable binary only supports IEEE 754 standardized floating point");

        ar.save_binary<sizeof(TT)>(bd.data, static_cast<std::size_t>(bd.size));
    }

    template <class T> inline
    void CEREAL_LOAD_FUNCTION_NAME(SimulinkInputArchive & ar, cereal::BinaryData<T>& bd){
        typedef typename std::remove_pointer<T>::type TT;
        static_assert(!std::is_floating_point<TT>::value ||
                    (std::is_floating_point<TT>::value && std::numeric_limits<TT>::is_iec559),
                    "Portable binary only supports IEEE 754 standardized floating point");

        ar.load_binary<sizeof(TT)>(bd.data, static_cast<std::size_t>(bd.size));
    }
}

CEREAL_REGISTER_ARCHIVE(simulink::SimulinkOutputArchive)
CEREAL_REGISTER_ARCHIVE(simulink::SimulinkInputArchive)

CEREAL_SETUP_ARCHIVE_TRAITS(simulink::SimulinkInputArchive, simulink::SimulinkOutputArchive)