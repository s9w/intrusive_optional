#pragma once

#include <exception>
#include <utility>
#include <optional>


namespace io
{
#define SWL_MOV(x) static_cast<std::remove_reference_t<decltype(x)>&&>(x)
#define SWL_FWD(x) static_cast<decltype(x)&&>(x)

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
   private:
      value_type m_value;

   public:

      // Constructors: (1)
      constexpr intrusive_optional() noexcept
         : m_value(null_value_param)
      { }

      constexpr intrusive_optional(std::nullopt_t) noexcept
         : m_value(null_value_param)
      { }


      // Constructors: (2)
      constexpr intrusive_optional(const intrusive_optional&)
         requires (std::is_copy_constructible_v<value_type> && std::is_trivially_copy_constructible_v<value_type>) = default;

      constexpr intrusive_optional(const intrusive_optional& other)
         requires (std::is_copy_constructible_v<value_type> && std::is_trivially_copy_constructible_v<value_type> == false)
      {
          this->construct_from_optional(other);
      }


      // Constructors: (3)
      constexpr intrusive_optional(intrusive_optional&&)
         requires std::is_trivially_move_constructible_v<value_type> = default;

      constexpr intrusive_optional(intrusive_optional&& other)
         noexcept(std::is_nothrow_move_constructible_v<value_type>)
         requires (std::is_move_constructible_v<value_type> && std::is_trivially_move_constructible_v<value_type> == false)
      {
         this->construct_from_optional(SWL_FWD(other));
      }


      // This is the requirement under the 4) and 5) constructors
      //template <class T, class U>
      //static inline constexpr bool requirement_4_and_5 =
      //   std::is_constructible_v<T, const U&>
      //   && std::is_constructible_v<T, std::optional<U>&> == false
      //   && std::is_constructible_v<T, const std::optional<U>&> == false
      //   && std::is_constructible_v<T, std::optional<U>&&> == false
      //   && std::is_constructible_v<T, const std::optional<U>&&> == false
      //   && std::is_convertible_v<std::optional<U>&, T> == false
      //   && std::is_convertible_v<const std::optional<U>&, T> == false
      //   && std::is_convertible_v<std::optional<U>&&, T> == false
      //   && std::is_convertible_v<const std::optional<U>&&, T> == false;

      // Constructors: (4, 5)
      // ignored


      // Constructors: (6)
      template<typename ... Args>
      constexpr explicit intrusive_optional(std::in_place_t, Args&&... args)
      {
         this->construct_from(SWL_FWD(args)...);
      }

      // Constructors: (7)
      //template <class U, class... Args>
      //constexpr explicit intrusive_optional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
      //   //requires std::is_constructible_v<value_type, std::initializer_list<U>&, Args...>
      //{
      //   this->construct_from(ilist, SWL_FWD(args)...);
      //}


      // Constructor (8)
      // Can't be implemented due to compile-time requirements






      // Constructor helpers
      template <class opt_type>
      constexpr auto construct_from_optional(opt_type&& opt) -> void
      {
         if (this->has_value())
         {
            this->construct_from(*SWL_FWD(opt));
         }
      }

      template <class... Args>
      constexpr auto construct_from(Args&&... args) -> void
      {
         std::construct_at(std::addressof(m_value), static_cast<Args&&>(args)...);
      }






      // Destructors
      constexpr ~intrusive_optional() requires std::is_trivially_destructible_v<value_type> = default;

      constexpr ~intrusive_optional() requires (std::is_trivially_destructible_v<value_type> == false)
      {
         if (this->has_value() == false)
            return;
         m_value.~value_type();
      }



      // operator= (1, 2 and 3) are unnecessary to explicitly implement

      // operator= (4)
      // can't implement due to requirements can't be checked compile-time

      // operator= (5, 6 not implemented







      // Observers: operator->
      [[nodiscard]] constexpr auto operator->() -> value_type*
      {
         return ::std::addressof(this->m_value);
      }

      [[nodiscard]] constexpr auto operator->() const -> const value_type*
      {
         return ::std::addressof(this->m_value);
      }



      // Observers: operator*
      [[nodiscard]] constexpr auto operator*() const& -> const value_type&
      {
         return this->m_value;
      }

      [[nodiscard]] constexpr auto operator*() & -> value_type&
      {
         return this->m_value;
      }

      [[nodiscard]] constexpr auto operator*() && -> value_type&&
      {
         return ::std::move(this->m_value);
      }

      [[nodiscard]] constexpr auto operator*() const&& -> const value_type&&
      {
         return ::std::move(this->m_value);
      }



      // Observers: operator bool
      explicit constexpr operator bool() const noexcept
      {
         return this->has_value();
      }



      // Observers: has_value
      [[nodiscard]] constexpr auto has_value() const noexcept -> bool
      {
         const bool is_equal_to_null = this->m_value == null_value;
         return is_equal_to_null == false;
      }



      // Observers: value
      [[nodiscard]] constexpr auto value() const & -> const value_type&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() & -> value_type&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() && -> value_type&&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }

      [[nodiscard]] constexpr auto value() const && -> const value_type&&
      {
         if (this->has_value() == false)
         {
            throw bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }



      // Observers: value_or
      template <class U> requires (std::is_copy_constructible_v<value_type> && std::is_convertible_v<U&&, value_type>)
         [[nodiscard]] constexpr auto value_or(U&& right) const& -> value_type
      {
         if (this->has_value())
         {
            return this->m_value;
         }

         return static_cast<value_type>(::std::forward<U>(right));
      }

      template <class U> requires (std::is_move_constructible_v<value_type>&& std::is_convertible_v<U&&, value_type>)
         [[nodiscard]] constexpr auto value_or(U&& right) && -> value_type
      {
         if (this->has_value())
         {
            return ::std::move(this->m_value);
         }

         return static_cast<value_type>(::std::forward<U>(right));
      }



      // Modifiers: swap
      constexpr void swap(intrusive_optional& right)
         noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_swappable_v<value_type>)
         requires std::is_move_constructible_v<value_type>
      {
         if (this->has_value() == false && right.has_value() == false)
         {
            return;
         }
         if (this->has_value() && right.has_value())
         {
            ::std::swap(this->m_value, right.m_value);
            return;
         }
         intrusive_optional& source = this->has_value() ? *this : right;
         intrusive_optional& target = this->has_value() ? right : *this;
         std::construct_at(std::addressof(target), *source);
         source.reset();
      }



      // Modifiers: reset
      constexpr auto reset() noexcept -> void
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



      // Modifiers: emplace
      // TODO


   }; // intrusive_optional


   // make_optional (1) not implemented because implicit argument deduction from parameter no longer possible

   // make_optional (2)
   template <auto T0, class... Args>
   [[nodiscard]] constexpr auto make_optional(Args&&... args) {
      return intrusive_optional<T0>{std::in_place, std::forward<Args>(args)...};
   }

   // make_optional (3)
   // initializer_list are stupids
   //template <auto T0, class U, class... Args>
   //constexpr auto make_optional(std::initializer_list<U> il, Args&&... args) {
   //   return intrusive_optional<T0> {std::in_place, il, SWL_FWD(args)...};
   //}





   template <auto null_value, typename T = std::decay_t<decltype(null_value)>>
   requires (std::is_move_constructible_v<T>&& std::is_swappable_v<T>)
      constexpr auto swap(intrusive_optional<null_value>& x, intrusive_optional<null_value>& y)
      noexcept(noexcept(x.swap(y)))
   -> void
   {
      x.swap(y);
   }


} // namespace io


// TODO hash
