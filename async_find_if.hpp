#pragma once
// async_find_if.hpp
#include <cassert>
#include <future>
#include <vector>
#include "find_if.hpp"

namespace eecs390
{

  // REQUIRES: Iterator must be a LegacyRandomAccessIterator (see
  //           https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator);
  //           begin must be before or equal to end;
  //           pred must be invocable on each element in [begin, end);
  //           num_tasks > 0;
  //           (end - begin) must be a multiple of num_tasks
  // EFFECTS: Returns an iterator to the first element in [begin, end)
  //          for which pred returns a true value, or end if no such
  //          element exists.
  // NOTE: Uses num_tasks tasks to search for the result concurrently,
  //       invoking eecs390::find_if on each task.
  // HINT: You may need to explicitly instantiate find_if, e.g.
  //       find_if<Iterator, Predicate>.
  template <typename Iterator, typename Predicate>
  Iterator async_find_if(Iterator begin, Iterator end, Predicate pred,
                         int num_tasks)
  {
    std::size_t size = end - begin;
    assert(size % num_tasks == 0);
    std::size_t count_per_task = size / num_tasks;

    std::vector<std::future<Iterator>> results;
    // MY CODE:
    // 1) make threads
    Iterator cur = begin;
    results.reserve(num_tasks);
    for (int i = 0; i < num_tasks; i++)
    {
      Iterator next = cur + count_per_task;
      results.push_back(std::async(
          std::launch::async, find_if<Iterator, Predicate>, cur, next, pred));
      cur = next;
    }
    // 2) check results:
    cur = begin;
    for (int i = 0; i < num_tasks; i++)
    {
      Iterator next = cur + count_per_task;
      Iterator gotten = results[i].get();
      if (gotten != next)
      {
        return gotten;
      }
      cur = next;
    }
    return end;
  }

}
