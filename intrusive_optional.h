#pragma once

#include <utility>
#include <optional>
#include <tuple> // should be free from <optional>


namespace io
{
#define SWL_MOV(x) static_cast<std::remove_reference_t<decltype(x)>&&>(x)
#define SWL_FWD(x) static_cast<decltype(x)&&>(x)

   struct unintentionally_null final : std::exception {
      [[nodiscard]] auto what() const noexcept -> const char* override
      {
         return "The value of this intrusive_optional was set to the declared null_value unintentionally.";
      }
   };

   enum class safety_mode{unsafe, safe};

   // intrusive_optional requires constexpr null-value and constexpr copy constructor
   template<auto null_value_param, safety_mode mode = safety_mode::unsafe>
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


      // Requirement for constructors (4) and (5)
      template <auto U0, typename U = std::decay_t<decltype(U0)>>
      static inline constexpr bool requirement_4_and_5 =
         std::is_constructible_v<value_type, const U&>
         && std::is_constructible_v<value_type, intrusive_optional<U0>&> == false
         && std::is_constructible_v<value_type, const intrusive_optional<U0>&> == false
         && std::is_constructible_v<value_type, intrusive_optional<U0>&&> == false
         && std::is_constructible_v<value_type, const intrusive_optional<U0>&&> == false
         && std::is_convertible_v<intrusive_optional<U0>&, value_type> == false
         && std::is_convertible_v<const intrusive_optional<U0>&, value_type> == false
         && std::is_convertible_v<intrusive_optional<U0>&&, value_type> == false
         && std::is_convertible_v<const intrusive_optional<U0>&&, value_type> == false;

      // Constructors: (4)
      template <auto U0, typename U = std::decay_t<decltype(U0)>> requires requirement_4_and_5<U0>
      constexpr explicit(std::is_convertible_v<const U&, value_type> == false) intrusive_optional(const intrusive_optional<U0>& other)
      {
         this->construct_from_optional(other);
      }


      // Constructors: (5)
      template <auto U0, typename U = std::decay_t<decltype(U0)>> requires requirement_4_and_5<U0>
      constexpr explicit(std::is_convertible_v<U, value_type> == false) intrusive_optional(intrusive_optional<U0>&& other)
      {
         this->construct_from_optional(SWL_FWD(other));
      }


      // Constructors: (6)
      template<typename ... Args>
      constexpr explicit intrusive_optional(std::in_place_t, Args&&... args)
      {
         this->construct_from(SWL_FWD(args)...);
         ensure_not_zero();
      }


      // Constructors: (7)
      template <class U, class... Args>
      constexpr explicit intrusive_optional(std::in_place_t, std::initializer_list<U> ilist, Args&&... args)
         requires std::is_constructible_v<value_type, std::initializer_list<U>&, Args...>
      {
         this->construct_from(ilist, SWL_FWD(args)...);
         ensure_not_zero();
      }


      // Constructor (8)
      template <class U = value_type>
      constexpr explicit(not std::is_convertible_v<U, value_type>) intrusive_optional(U&& u)
         requires (std::is_constructible_v<value_type, U>
            && std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> == false
            && std::is_same_v<std::remove_cvref_t<U>, intrusive_optional> == false)
      {
         this->construct_from(SWL_FWD(u));
         ensure_not_zero();
      }






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


      template <typename Opt>
      constexpr auto assign_from_optional(Opt&& other) -> void
      {
         // "If both *this and other do not contain a value, the function has no effect."
         if(this->has_value() == false && other.has_value() == false)
         {
            return;
         }

         // "If *this contains a value, but other does not, then the contained value is destroyed
         // by calling its destructor. *this does not contain a value after the call."
         if(this->has_value() && other.has_value() == false)
         {
            this->reset_impl();
         }

         // "If other contains a value, then depending on whether *this contains a value, the
         // contained value is either direct-initialized or assigned from *other (2) or
         // std::move(*other) (3). Note that a moved-from optional still contains a value."
         if(other.has_value())
         {
            if(this->has_value())
            {
               **this = *SWL_FWD(other);
            }
            else
            {
               this->construct_from(*SWL_FWD(other));
            }
         }
      }


      constexpr auto ensure_not_zero() const -> void
      {
         if constexpr(mode == safety_mode::safe)
         {
            if (this->has_value() == false)
            {
               throw unintentionally_null{};
            }
         }
      }


      // Destructors
      constexpr ~intrusive_optional() requires std::is_trivially_destructible_v<value_type> = default;

      constexpr ~intrusive_optional() requires (std::is_trivially_destructible_v<value_type> == false)
      {
         if (this->has_value() == false)
            return;
         m_value.~value_type();
      }


      // operator= conditions
      static constexpr inline bool assignment_2_cond = std::is_copy_constructible_v<value_type> && std::is_copy_assignable_v<value_type>;
      static constexpr inline bool assignment_2_trivial_cond = std::is_trivially_copy_constructible_v<value_type> && std::is_trivially_copy_assignable_v<value_type> && std::is_trivially_destructible_v<value_type>;
      static constexpr inline bool assignment_3_cond = std::is_move_constructible_v<value_type> && std::is_move_assignable_v<value_type>;
      static constexpr inline bool assignment_3_trivial_cond = std::is_trivially_move_constructible_v<value_type> && std::is_trivially_move_assignable_v<value_type> && std::is_trivially_destructible_v<value_type>;

      // operator= (2)
      constexpr auto operator=(const intrusive_optional&) -> intrusive_optional&
          = default;

      constexpr auto operator=(const intrusive_optional& other) -> intrusive_optional&
         requires assignment_2_cond
      {
         this->assign_from_optional(other);
         return *this;
      }


      // operator= (3)
      constexpr auto operator=(intrusive_optional&&)
         noexcept(std::is_nothrow_move_assignable_v<value_type>&& std::is_nothrow_move_constructible_v<value_type>)
         -> intrusive_optional&
         requires (assignment_3_cond && assignment_3_trivial_cond)
         = default;

      constexpr auto operator=(intrusive_optional&& other)
      noexcept(std::is_nothrow_move_assignable_v<value_type> && std::is_nothrow_move_constructible_v<value_type>)
      -> intrusive_optional&
         requires assignment_3_cond
      {
         this->assign_from_optional(SWL_FWD(other));
         return *this;
      }


      // operator= (4)
      template <class U = value_type>
      requires
         (std::is_same_v<std::remove_cvref_t<U>, intrusive_optional> == false
            && (std::is_scalar_v<value_type> == false || std::is_same_v<value_type, std::decay_t<U>> == false)
            && std::is_constructible_v<value_type, U>
            && std::is_assignable_v<value_type&, U>)
         constexpr auto operator=(U&& u) -> intrusive_optional&
      {
         if (this->has_value())
         {
            this->m_value = SWL_FWD(u);
         }
         else
         {
            this->construct_from(SWL_FWD(u));
         }
         ensure_not_zero();
         return *this;
      }

      // TODO operator= (5) and (6)







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
         requires(mode == safety_mode::unsafe)
      {
         return this->m_value;
      }

      [[nodiscard]] constexpr auto operator*() && -> value_type&&
         requires(mode == safety_mode::unsafe)
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
         if (this->has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() & -> value_type&
         requires(mode == safety_mode::unsafe)
      {
         if (this->has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return this->m_value;
      }

      [[nodiscard]] constexpr auto value() && -> value_type&&
         requires(mode == safety_mode::unsafe)
      {
         if (this->has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }

      [[nodiscard]] constexpr auto value() const && -> const value_type&&
      {
         if (this->has_value() == false)
         {
            throw std::bad_optional_access{};
         }
         return ::std::move(this->m_value);
      }



      // Observers: value_or
      template <class U> requires (std::is_copy_constructible_v<value_type> && std::is_convertible_v<U&&, value_type>)
         [[nodiscard]] constexpr auto value_or(U&& default_value) const& -> value_type
      {
         if (this->has_value())
         {
            return this->m_value;
         }

         return static_cast<value_type>(::std::forward<U>(default_value));
      }

      template <class U> requires (std::is_move_constructible_v<value_type>&& std::is_convertible_v<U&&, value_type>)
         [[nodiscard]] constexpr auto value_or(U&& default_value) && -> value_type
      {
         if (this->has_value())
         {
            return ::std::move(this->m_value);
         }

         return static_cast<value_type>(::std::forward<U>(default_value));
      }



      // Modifiers: swap
      constexpr void swap(intrusive_optional& other)
         noexcept(std::is_nothrow_move_constructible_v<value_type> && std::is_nothrow_swappable_v<value_type>)
         requires std::is_move_constructible_v<value_type>
      {
         if (this->has_value() == false && other.has_value() == false)
         {
            return;
         }
         if (this->has_value() && other.has_value())
         {
            ::std::swap(this->m_value, other.m_value);
            return;
         }
         intrusive_optional& source = this->has_value() ? *this : other;
         intrusive_optional& target = this->has_value() ? other : *this;
         std::construct_at(std::addressof(target), *source);
         source.reset();
      }


      // Modifiers: emplace
      template <class... Args>
      requires std::is_constructible_v<value_type, Args...>
         constexpr auto emplace(Args&&... args) -> void
      {
         this->reset();
         this->construct_from(SWL_FWD(args)...);
      }

      template <class U, class... Args>
      requires std::is_constructible_v<value_type, std::initializer_list<U>&, Args...>
         constexpr auto emplace(std::initializer_list<U> ilist, Args&&... args) -> void
      {
         this->reset();
         this->construct_from(ilist, SWL_FWD(args)...);
      }



      // Modifiers: reset
      constexpr auto reset() noexcept -> void
      {
         if (this->has_value() == false)
         {
            return;
         }

         this->reset_impl();
      }


      constexpr auto reset_impl() noexcept -> void
      {
         if constexpr (std::is_trivially_destructible_v<value_type> == false)
         {
            this->m_value.~value_type();
         }
         this->m_value = null_value;
      }


   }; // intrusive_optional


   // Non-member functions; comparisons (1-6)
   template <auto T, auto U>
   constexpr auto operator==(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs == *rhs); }
   {
      if (bool(lhs) != bool(rhs))
         return false;
      if (bool(lhs) == false)
         return true;
      return *lhs == *rhs;
   }

   // comparison (2)
   template <auto T, auto U>
   constexpr auto operator!=(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs != *rhs); }
   {
      if (bool(lhs) != bool(rhs))
         return true;
      if (bool(lhs) == false)
         return false;
      return *lhs != *rhs;
   }

   // comparison (3)
   template <auto T, auto U>
   constexpr auto operator<(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs < *rhs); }
   {
      if (bool(rhs) == false)
         return false;
      if (bool(lhs) == false)
         return true;
      return *lhs < *rhs;
   }

   // comparison (4)
   template <auto T, auto U>
   constexpr auto operator<=(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs <= *rhs); }
   {
      if (bool(lhs) == false)
         return true;
      if (bool(rhs) == false)
         return false;
      return *lhs <= *rhs;
   }
   

   // comparison (5)
   template <auto T, auto U>
   constexpr auto operator>(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs > * rhs); }
   {
      if (bool(lhs) == false)
         return false;
      if (bool(rhs) == false)
         return true;
      return *lhs > *rhs;
   }

   // comparison (6)
   template <auto T, auto U>
   constexpr auto operator>=(const intrusive_optional<T>& lhs, const intrusive_optional<U>& rhs) -> bool
      requires requires { bool(*lhs >= *rhs); }
   {
      if (bool(lhs) == false)
         return false;
      if (bool(rhs) == false)
         return true;
      return *lhs >= *rhs;
   }

   // TODO other comparison operators


   // make_optional (1) not implemented because automatic deduction of the full type isn't possible
   // with intrusive_optional since it requires a value and not just a type to instantiate.

   // make_optional (2)
   template <auto T0, class... Args>
   [[nodiscard]] constexpr auto make_optional(Args&&... args) -> intrusive_optional<T0>
   {
      return intrusive_optional<T0>{std::in_place, std::forward<Args>(args)...};
   }

   // make_optional (3)
   template <auto T0, class U, class... Args>
   constexpr auto make_optional(std::initializer_list<U> il, Args&&... args) -> intrusive_optional<T0>
   {
      return intrusive_optional<T0> {std::in_place, il, SWL_FWD(args)...};
   }





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
