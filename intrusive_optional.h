#pragma once

#include <exception>
#include <utility>


namespace io
{

   class bad_optional_access : public ::std::exception {
   public:
      [[nodiscard]] auto what() const noexcept -> const char* override
      {
         return "Bad intrusive_optional access";
      }
   };

   template<auto null_value_param>
   struct intrusive_optional
   {
      using value_type = std::decay_t<decltype(null_value_param)>;

      constexpr inline static value_type null_value = null_value_param;
      value_type m_value = null_value;

      constexpr intrusive_optional() noexcept = default;


      [[nodiscard]] constexpr auto has_value() const noexcept -> bool
      {
         const bool is_equal_to_null = m_value == null_value;
         return is_equal_to_null == false;
      }
      [[nodiscard]] constexpr auto operator*() noexcept -> value_type&
      {
         return m_value;
      }
      [[nodiscard]] constexpr auto operator*() const noexcept -> const value_type&
      {
         return m_value;
      }
      [[nodiscard]] constexpr auto value() -> value_type&
      {
         if (has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return m_value;
      }
      [[nodiscard]] constexpr auto value() const -> const value_type&
      {
         if (has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return m_value;
      }
   };


   template <class T>
   constexpr auto make_optional(T&& v) {
      return intrusive_optional<std::decay_t<T>>{SWL_FWD(v)};
   }

   template <class T, class... Args>
   constexpr auto make_optional(Args&&... args) {
      return intrusive_optional<T>{std::in_place, SWL_FWD(args)...};
   }


}