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
   private:
      value_type m_value = null_value;
   public:

      // Constructors
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



      // Destructors
      constexpr ~intrusive_optional() requires std::is_trivially_destructible_v<value_type> = default;

      constexpr ~intrusive_optional() requires (std::is_trivially_destructible_v<value_type> == false)
      {
         if (m_value == false)
            return;
         m_value.~value_type();
      }



      static constexpr bool has_move_assign = std::is_move_assignable_v<value_type> && std::is_move_constructible_v<value_type>;
      static constexpr bool has_trivial_move_assign = std::is_trivially_move_assignable_v<value_type> && std::is_trivially_move_constructible_v<value_type>;
      static constexpr bool has_trivial_copy = std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_copy_constructible_v<value_type>;
      static constexpr bool has_copy = std::is_copy_assignable_v<value_type> && std::is_copy_constructible_v<value_type>;

      // operator=
      constexpr intrusive_optional& operator=(const intrusive_optional& o) requires has_trivial_copy = default;
      constexpr intrusive_optional& operator=(const intrusive_optional& o) requires (has_copy && has_trivial_copy == false)
      {
         this->m_value = o.m_value;
         return *this;
         // return this->assign_from_optional(o);
      }

      constexpr intrusive_optional& operator=(intrusive_optional&& o) requires has_trivial_move_assign = default;
      constexpr intrusive_optional& operator=(intrusive_optional&& o) requires (has_move_assign && has_trivial_move_assign == false)
      {
         this->m_value = std::move(o.m_value);
         return *this;
         // return this->assign_from_optional(SWL_FWD(o));
      }

      // template <class U = T>
      // constexpr intrusive_optional& operator=(U && u)
      //    requires
      //    (not std::is_same_v<std::remove_cvref_t<U>, optional>
      //       and not(std::is_scalar_v<T> and std::is_same_v<T, std::decay_t<U>>)
      //       and std::is_constructible_v<T, U>
      //       and std::is_assignable_v<T&, U>)
      // {
      //    // if (active)
      //    //    **this = SWL_FWD(u);
      //    // else
      //    //    this->construct_from(SWL_FWD(u));
      //    // return *this;
      // }



      // Observers: operator->
      [[nodiscard]] constexpr auto operator->()->value_type*
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

      [[nodiscard]] constexpr auto operator*() & ->value_type&
      {
         return this->m_value;
      }

      [[nodiscard]] constexpr auto operator*() && ->value_type&&
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
      [[nodiscard]] constexpr auto value() const& -> const value_type&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() & ->value_type&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() && ->value_type&&
      {
         if (this->has_value == false)
         {
            throw bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }

      [[nodiscard]] constexpr auto value() const&& -> const value_type&&
      {
         if (this->has_value() == false)
         {
            throw bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }



      // Observers: value_or
      template <class U> requires (std::is_copy_constructible_v<value_type>&& std::is_convertible_v<U&&, value_type>)
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
         noexcept(std::is_nothrow_move_constructible_v<value_type>&& std::is_nothrow_swappable_v<value_type>)
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
   };


   template <class T>
   [[nodiscard]] constexpr auto make_optional(T&& v) {
      return intrusive_optional<std::decay_t<T>>{std::forward<T>(v)};
   }

   template <class T, class... Args>
   [[nodiscard]] constexpr auto make_optional(Args&&... args) {
      return intrusive_optional<T>{std::in_place, std::forward<Args>(args)...};
   }

   template <auto null_value, typename T = std::decay_t<decltype(null_value)>>
   requires (std::is_move_constructible_v<T>&& std::is_swappable_v<T>)
      constexpr void swap(intrusive_optional<null_value>& x, intrusive_optional<null_value>& y)
      noexcept(noexcept(x.swap(y)))
   {
      x.swap(y);
   }


} // namespace io


// TODO hash
