#pragma once

#include <exception>
#include <utility>


namespace io
{

   struct bad_optional_access final : ::std::exception {
      [[nodiscard]] auto what() const noexcept -> const char* override
      {
         return "Bad intrusive_optional access";
      }
   };

   // requires constexpr null-value and constexpr copy constructor
   template<auto null_value_param>
   struct intrusive_optional
   {
      using value_type = std::decay_t<decltype(null_value_param)>;

      constexpr inline static value_type null_value{ null_value_param };
      value_type m_value = null_value;

      constexpr intrusive_optional() noexcept = default;
      constexpr intrusive_optional(const intrusive_optional& o) requires std::is_copy_constructible_v<value_type> = default;
      constexpr intrusive_optional(intrusive_optional&& o) requires std::is_move_constructible_v<value_type> = default;

      template<typename ... Args>
      constexpr explicit intrusive_optional(std::in_place_t, Args&&... args) : m_value(args...)
      {
         
      }

      constexpr intrusive_optional(const value_type& value) : m_value(value)
      {
         
      }

      constexpr intrusive_optional(value_type&& value) : m_value(std::move(value))
      {
         
      }

      constexpr ~intrusive_optional() requires std::is_trivially_destructible_v<value_type> = default;

      constexpr ~intrusive_optional() requires (std::is_trivially_destructible_v<value_type> == false)
      {
         if (m_value == false)
            return;
         m_value.~value_type();
      }


      [[nodiscard]] constexpr auto value() const &  -> const value_type&;
      [[nodiscard]] constexpr auto value()       &  ->       value_type&;
      [[nodiscard]] constexpr auto value()       && ->       value_type&&;
      [[nodiscard]] constexpr auto value() const && -> const value_type&&;

      [[nodiscard]] constexpr auto operator*() const &  -> const value_type&;
      [[nodiscard]] constexpr auto operator*()       &  ->       value_type&;
      [[nodiscard]] constexpr auto operator*()       && ->       value_type&&;
      [[nodiscard]] constexpr auto operator*() const && -> const value_type&&;

      explicit constexpr operator bool() const noexcept;
                    constexpr auto reset() noexcept -> void;
      [[nodiscard]] constexpr auto has_value() const noexcept -> bool;
      [[nodiscard]] constexpr auto operator->()       ->       value_type*;
      [[nodiscard]] constexpr auto operator->() const -> const value_type*;
   };


   template <class T>
   constexpr auto make_optional(T&& v) {
      return intrusive_optional<std::decay_t<T>>{std::forward<T>(v)};
   }

   template <class T, class... Args>
   constexpr auto make_optional(Args&&... args) {
      return intrusive_optional<T>{std::in_place, std::forward<Args>(args)...};
   }


}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator->() -> value_type*
{
   return std::addressof(this->m_value);
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator->() const -> const value_type*
{
   return std::addressof(this->m_value);
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::value() const & -> const value_type&
{
   if (this->has_value == false)
   {
      throw bad_optional_access{};
   }

   return this->m_value;
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::value() & -> value_type&
{
   if (this->has_value == false)
   {
      throw bad_optional_access{};
   }

   return this->m_value;
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::value() && -> value_type&&
{
   if (this->has_value == false)
   {
      throw bad_optional_access{};
   }

   return ::std::move(this->m_value);
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::value() const && -> const value_type&&
{
   if (this->has_value() == false)
   {
      throw bad_optional_access{};
   }

   return ::std::move(this->m_value);
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator*() const & -> const value_type&
{
   return this->m_value;
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator*() & -> value_type&
{
   return this->m_value;
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator*() && -> value_type&&
{
   return ::std::move(this->m_value);
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::operator*() const && -> const value_type&&
{
   return ::std::move(this->m_value);
}


template <auto null_value_param>
constexpr io::intrusive_optional<null_value_param>::operator bool() const noexcept
{
   return this->has_value();
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::reset() noexcept -> void
{
   if (this->has_value() == false)
   {
      return;
   }

   if constexpr (std::is_trivially_destructible_v<value_type> == false)
   {
      this->m_value.~value_type();
   }
   this->m_value = null_value;
}


template <auto null_value_param>
constexpr auto io::intrusive_optional<null_value_param>::has_value() const noexcept -> bool
{
   const bool is_equal_to_null = this->m_value == null_value;
   return is_equal_to_null == false;
}


// TODO hash
