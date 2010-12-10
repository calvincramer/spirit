/*==============================================================================
    Copyright (c) 2001-2010 Joel de Guzman
    Copyright (c) 2010      Bryce Lelbach

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
==============================================================================*/

#if !defined(BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR)
#define BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/range/iterator_range.hpp>

namespace boost {
namespace spirit {

////////////////////////////////////////////////////////////////////////////////
// line_pos_iterator: a lightweight line position iterator. This iterator
// adapter only stores the current line number, nothing else. Unlike spirit
// classic's position_iterator, it does not store the column number and does not
// need an end iterator. The current column can be computed, if needed. Some
// line oriented utilities are provided including computation of the current
// column.
////////////////////////////////////////////////////////////////////////////////

//[line_pos_iterator
template<class Iterator>
class line_pos_iterator: public boost::iterator_adaptor<
  line_pos_iterator<Iterator>, // Derived
  Iterator,                    // Base
  boost::use_default,          // Value
  boost::forward_traversal_tag // CategoryOrTraversal
> {
 public:
  line_pos_iterator (void);

  explicit line_pos_iterator (Iterator);

  std::size_t position (void) const;

 private:
  friend class boost::iterator_core_access;

  void increment (void);

  std::size_t line; /*< The line position. >*/
  typename std::iterator_traits<Iterator>::value_type prev;
};
//]

template<class Iterator>
line_pos_iterator<Iterator>::line_pos_iterator (void):
  line_pos_iterator::iterator_adaptor_(), line(1), prev(0) { }

template<class Iterator>
line_pos_iterator<Iterator>::line_pos_iterator (Iterator base):
  line_pos_iterator::iterator_adaptor_(base), line(1), prev(0) { }

template<class Iterator>
std::size_t line_pos_iterator<Iterator>::position (void) const {
  return line;
}
 
template<class Iterator>
void line_pos_iterator<Iterator>::increment (void) {
  typename std::iterator_traits<Iterator>::reference ref = *(this->base());

  switch (ref) {
    case '\r':
      if (prev != '\n')
        ++line;
      break;
    case '\n':
      if (prev != '\r')
        ++line;
      break;
    default:
      break;
  }

  prev = ref;
  ++this->base_reference();
}

//[line_pos_iterator_utilities
template<class Iterator>
inline std::size_t get_line (Iterator i); /*< Get the line position. Returns -1
                                              if Iterator is not a
                                              line_pos_iterator. >*/

template<class Iterator>
inline Iterator get_line_start (Iterator lower_bound,
                                Iterator current); /*< Get an iterator to the
                                                       beginning of the line.
                                                       Applicable to any
                                                       iterator. >*/

template<class Iterator>
inline iterator_range<Iterator>
get_current_line (Iterator lower_bound,
                  Iterator current,
                  Iterator upper_bound); /*< Get the iterator range containing
                                             the current line. Applicable to
                                             any iterator. >*/

template<class Iterator>
inline std::size_t get_column (Iterator lower_bound,
                               Iterator current,
                               std::size_t tabs = 4); /*< Get the current
                                                          column. Applicable to
                                                          any iterator. >*/
//]

template<class Iterator>
inline std::size_t get_line (Iterator i) {
  return -1;
}

template<class Iterator>
inline std::size_t get_line (line_pos_iterator<Iterator> i) {
  return i.position();
}

template<class Iterator>
inline Iterator get_line_start (Iterator lower_bound, Iterator current) {
  Iterator latest = lower_bound;

  for (Iterator i = lower_bound; i != current; ++i) {
    switch (*i) {
      case '\r':
      case '\n':
        latest = i;
    }
  }

  return latest;
}

template<class Iterator>
inline iterator_range<Iterator>
get_current_line (Iterator lower_bound,
                  Iterator current,
                  Iterator upper_bound) {
  Iterator first = get_line_start(lower_bound, current);
  Iterator last = get_line_start(current, upper_bound);

  if (last == current)
    last = upper_bound;

  return iterator_range<Iterator>(first, last);
}

template<class Iterator>
inline std::size_t get_column (Iterator lower_bound,
                               Iterator current,
                               std::size_t tabs) {
  std::size_t column = 1;
  Iterator first = get_line_start(lower_bound, current);

  for (Iterator i = first; i != current; ++i) {
    switch (*i) {
      case '\t':
        column += tabs - (column - 1) % tabs;
        break;
      default:
        ++column;
    }
  }

  return column;
}

} // spirit
} // boost

#endif // BOOST_SPIRIT_SUPPORT_LINE_POS_ITERATOR
