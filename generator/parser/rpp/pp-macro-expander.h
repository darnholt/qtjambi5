/****************************************************************************
**
** Copyright (C) 1992-2009 Nokia. All rights reserved.
** Copyright 2005 Roberto Raggi <roberto@kdevelop.org>
**
** This file is part of Qt Jambi.
**
** ** $BEGIN_LICENSE$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $END_LICENSE$

**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef PP_MACRO_EXPANDER_H
#define PP_MACRO_EXPANDER_H

#include <string>

#include "pp-macro.h"
#include "pp-scanner.h"
#include "pp-environment.h"
#include "pp-internal.h"

namespace rpp {

    struct pp_frame {
        pp_macro *expanding_macro;
        std::vector<std::string> *actuals;

        pp_frame(pp_macro *__expanding_macro, std::vector<std::string> *__actuals) :
                expanding_macro(__expanding_macro), actuals(__actuals) {}
    };

    class pp_macro_expander {
            pp_environment &env;
            pp_frame *frame;

            pp_skip_number skip_number;
            pp_skip_identifier skip_identifier;
            pp_skip_string_literal skip_string_literal;
            pp_skip_char_literal skip_char_literal;
            pp_skip_argument skip_argument;
            pp_skip_comment_or_divop skip_comment_or_divop;
            pp_skip_blanks skip_blanks;
            pp_skip_whitespaces skip_whitespaces;

            std::string const *resolve_formal(pp_fast_string const *__name);

        public: // attributes
            int lines;
            int generated_lines;

        public:
            pp_macro_expander(pp_environment &__env, pp_frame *__frame = 0) :
                    env(__env), frame(__frame), lines(0), generated_lines(0) {}

            template <typename _InputIterator>
            _InputIterator skip_argument_variadics(std::vector<std::string> const &actuals, pp_macro *macro,
                                                   _InputIterator first, _InputIterator last) {
                _InputIterator arg_end = skip_argument(first, last);

                while (macro->variadics && first != arg_end && arg_end != last && *arg_end == ','
                        && (actuals.size() + 1) == macro->formals.size()) {
                    arg_end = skip_argument(++arg_end, last);
                }

                return arg_end;
            }

            template <typename _InputIterator, typename _OutputIterator>
            _InputIterator operator()(_InputIterator first, _InputIterator last, _OutputIterator result) {
                generated_lines = 0;
                first = skip_blanks(first, last);
                lines = skip_blanks.lines;

                while (first != last) {
                    if (*first == '\n') {
                        *result++ = *first;
                        ++lines;

                        first = skip_blanks(++first, last);
                        lines += skip_blanks.lines;

                        if (first != last && *first == '#')
                            break;
                    } else if (*first == '#') {
                        first = skip_blanks(++first, last);
                        lines += skip_blanks.lines;

                        _InputIterator end_id = skip_identifier(first, last);

                        char name_buffer[512], *cp = name_buffer;
                        std::size_t name_size = end_id - first;
                        assert(name_size < 511); //crude hack to test the size, needs testing
                        std::copy(first, end_id, cp);
                        name_buffer[name_size] = '\0';

                        pp_fast_string fast_name(name_buffer, name_size);

                        if (std::string const *actual = resolve_formal(&fast_name)) {
                            *result++ = '\"';

                            for (std::string::const_iterator it = skip_whitespaces(actual->begin(), actual->end());
                                    it != actual->end(); ++it) {
                                if (*it == '"') {
                                    *result++ = '\\';
                                    *result++ = *it;
                                } else if (*it == '\n') {
                                    *result++ = '"';
                                    *result++ = '\n';
                                    *result++ = '"';
                                } else
                                    *result++ = *it;
                            }

                            *result++ = '\"';
                            first = end_id;
                        } else
                            *result++ = '#'; // ### warning message?
                    } else if (*first == '\"') {
                        _InputIterator next_pos = skip_string_literal(first, last);
                        lines += skip_string_literal.lines;
                        std::copy(first, next_pos, result);
                        first = next_pos;
                    } else if (*first == '\'') {
                        _InputIterator next_pos = skip_char_literal(first, last);
                        lines += skip_char_literal.lines;
                        std::copy(first, next_pos, result);
                        first = next_pos;
                    } else if (rpp::_PP_internal::comment_p(first, last)) {
                        first = skip_comment_or_divop(first, last);
                        int n = skip_comment_or_divop.lines;
                        lines += n;

                        while (n-- > 0)
                            *result++ = '\n';
                    } else if (pp_isspace(*first)) {
                        for (; first != last; ++first) {
                            if (*first == '\n' || !pp_isspace(*first))
                                break;
                        }

                        *result = ' ';
                    } else if (pp_isdigit(*first)) {
                        _InputIterator next_pos = skip_number(first, last);
                        lines += skip_number.lines;
                        std::copy(first, next_pos, result);
                        first = next_pos;
                    } else if (pp_isalpha(*first) || *first == '_') {
                        _InputIterator name_begin = first;
                        _InputIterator name_end = skip_identifier(first, last);
                        first = name_end; // advance

                        // search for the paste token
                        _InputIterator next = skip_blanks(first, last);
                        if (next != last && *next == '#') {
                            ++next;
                            if (next != last && *next == '#')
                                first = skip_blanks(++next, last);
                        }

                        // ### rewrite: not safe

                        std::ptrdiff_t name_size;
#if defined(__SUNPRO_CC)
                        std::distance(name_begin, name_end, name_size);
#else
                        name_size = std::distance(name_begin, name_end);
#endif
                        assert(name_size >= 0 && name_size < 512);

                        char name_buffer[512], *cp = name_buffer;
                        std::size_t size = name_end - name_begin;
                        std::copy(name_begin, name_end, cp);
                        name_buffer[size] = '\0';

                        pp_fast_string fast_name(name_buffer, name_size);

                        if (std::string const *actual = resolve_formal(&fast_name)) {
                            std::copy(actual->begin(), actual->end(), result);
                            continue;
                        }

                        static bool hide_next = false; // ### remove me

                        pp_macro *macro = env.resolve(name_buffer, name_size);
                        if (! macro || macro->hidden || hide_next) {
                            hide_next = ! strcmp(name_buffer, "defined");

                            if (size == 8 && name_buffer [0] == '_' && name_buffer [1] == '_') {
                                if (! strcmp(name_buffer, "__LINE__")) {
                                    char buf [16];
                                    char *end = buf + pp_snprintf(buf, 16, "%d", env.current_line + lines);

                                    std::copy(&buf [0], end, result);
                                    continue;
                                }

                                else if (! strcmp(name_buffer, "__FILE__")) {
                                    result++ = '"';
                                    std::copy(env.current_file.begin(), env.current_file.end(), result);      // ### quote
                                    result++ = '"';
                                    continue;
                                }
                            }

                            std::copy(name_begin, name_end, result);
                            continue;
                        }

                        if (! macro->function_like) {
                            pp_macro *new_macro = 0;

                            if (macro->definition) {
                                macro->hidden = true;

                                std::string tmp;
                                tmp.reserve(256);

                                pp_macro_expander expand_macro(env);
                                expand_macro(macro->definition->begin(), macro->definition->end(), std::back_inserter(tmp));
                                generated_lines += expand_macro.lines;

                                if (! tmp.empty()) {
                                    std::string::iterator begin_id = skip_whitespaces(tmp.begin(), tmp.end());
                                    std::string::iterator end_id = skip_identifier(begin_id, tmp.end());

                                    if (end_id == tmp.end()) {
                                        std::string id;
                                        id.assign(begin_id, end_id);

                                        std::size_t new_macro_size;
#if defined(__SUNPRO_CC)
                                        std::distance(begin_id, end_id, new_macro_size);
#else
                                        new_macro_size = std::distance(begin_id, end_id);
#endif
                                        new_macro = env.resolve(id.c_str(), new_macro_size);
                                    }

                                    if (! new_macro)
                                        std::copy(tmp.begin(), tmp.end(), result);
                                }

                                macro->hidden = false;
                            }

                            if (! new_macro)
                                continue;

                            macro = new_macro;
                        }

                        // function like macro
                        _InputIterator arg_it = skip_whitespaces(first, last);

                        if (arg_it == last || *arg_it != '(') {
                            std::copy(name_begin, name_end, result);
                            lines += skip_whitespaces.lines;
                            first = arg_it;
                            continue;
                        }

                        std::vector<std::string> actuals;
                        actuals.reserve(5);
                        ++arg_it; // skip '('

                        pp_macro_expander expand_actual(env, frame);

                        _InputIterator arg_end = skip_argument_variadics(actuals, macro, arg_it, last);
                        if (arg_it != arg_end) {
                            std::string actual(arg_it, arg_end);
                            actuals.resize(actuals.size() + 1);
                            actuals.back().reserve(255);
                            expand_actual(actual.begin(), actual.end(), std::back_inserter(actuals.back()));
                            arg_it = arg_end;
                        }

                        while (arg_it != last && *arg_end == ',') {
                            ++arg_it; // skip ','

                            arg_end = skip_argument_variadics(actuals, macro, arg_it, last);
                            std::string actual(arg_it, arg_end);
                            actuals.resize(actuals.size() + 1);
                            actuals.back().reserve(255);
                            expand_actual(actual.begin(), actual.end(), std::back_inserter(actuals.back()));
                            arg_it = arg_end;
                        }

                        assert(arg_it != last && *arg_it == ')');

                        ++arg_it; // skip ')'
                        first = arg_it;

#if 0 // ### enable me
                        assert((macro->variadics && macro->formals.size() >= actuals.size())
                               || macro->formals.size() == actuals.size());
#endif

                        pp_frame frame(macro, &actuals);
                        pp_macro_expander expand_macro(env, &frame);
                        macro->hidden = true;
                        expand_macro(macro->definition->begin(), macro->definition->end(), result);
                        macro->hidden = false;
                        generated_lines += expand_macro.lines;
                    } else
                        *result++ = *first++;
                }

                return first;
            }

    };

} // namespace rpp

#endif // PP_MACRO_EXPANDER_H
