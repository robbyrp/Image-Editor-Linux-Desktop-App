#!/usr/bin/python3 -u
# Copyright 2020 Darius Neatu (neatudarius@gmail.com)

from datetime import datetime, timedelta
import os
import re
import resource
import sys

LOG_INDENT = ''

# Define the global list of regex patterns
LOG_ALWAYS_DUMP_PATTERNS = [
   # skip all check_deps logs
   
   # add some make_build logs
   re.compile(r'make_build'),
   re.compile(r'Makefile'),
   re.compile(r'ERRORS'),
   re.compile('WARNINGS'),
   re.compile('failed'),

   # add some run_tasks, run_test_with_env_deps, check_test, logs
   re.compile(r'run_tasks'),
   re.compile(r'running task'),
   re.compile(r'running test .* grade'),
   re.compile(r'UPS'),
   re.compile(r'TLE'),
   re.compile(r'MLE'),
   re.compile(r'MEM_UPS'),
   re.compile(r'WA'),

   # add some check_style logs
   re.compile(r'check_style'),
   re.compile(r'.c:'),
   re.compile(r'.h:'),
   re.compile(r'style errors automatically detected'),

   # add some check_readme logs
   re.compile(r'check_readme'),
   re.compile(r'README'),

   # add some make_clean logs
   re.compile(r'make_clean'),
   re.compile(r'ERRORS'),
   re.compile('WARNINGS'),

   # add some final logs
   re.compile(r'Final grade:'),
   re.compile(r'Total:'), 
]

def delete_line_from_stdout():
    sys.stdout.write('\033[F')

def is_online_judge():
    return 'ONLINE_JUDGE' in os.environ

def should_always_log_message(*args, **kwargs):
    global LOG_ALWAYS_DUMP_PATTERNS

    message = ' '.join(map(str, args))
    for pattern in LOG_ALWAYS_DUMP_PATTERNS:
        if pattern.search(message):
            return True
    return False

def log(*args, **kwargs):
    global LOG_INDENT

    # Drop log if in online judge and message does not match any filter pattern.
    if is_online_judge() and not should_always_log_message(*args, **kwargs):
        return

    # Print with optional indent.
    if len(LOG_INDENT) > 0:
        print('{}'.format(LOG_INDENT), end='')
    print(*args, **kwargs)


def log_replace(*args, **kwargs):
    # Drop log if in online judge and message does not match any filter pattern.
    if is_online_judge() and not should_always_log_message(*args, **kwargs):
        return

    delete_line_from_stdout()
    log(*args, **kwargs)


def indent_log(levels=1):
    global LOG_INDENT
    for level in range(levels):
        LOG_INDENT += '\t'


def unindent_log(levels=1):
    global LOG_INDENT
    for level in range(levels):
        LOG_INDENT = LOG_INDENT[:-1]


def bug():
    assert False, 'BUG - please send an email to neatudarius@gmail.com with the log'


def extract_stdout(process):
    return process.stdout.decode("utf-8").rstrip()


def extract_stderr(process):
    return process.stderr.decode("utf-8").rstrip()


global_mem_bytes = None


def set_mem_bytes(bytes):
    global global_mem_bytes
    global_mem_bytes = bytes


def limit_process_memory():
    if global_mem_bytes is not None:
        resource.setrlimit(resource.RLIMIT_AS,
                           (global_mem_bytes, global_mem_bytes))


def print_legend():
    log('Legend:')
    indent_log()
    log('UPS       - Ups, program crashed')
    log('TLE       - Time Limit Exceed')
    log('MLE       - Memory Limit Exceed')
    log('MEM_UPS   - Memory leaks or errors')
    log('WA        - Wrong Answer (wrong or partial output)')
    log('OK        - Everything is OK')
    log('')
    unindent_log()


def run_and_measure(func):
    start = datetime.now()
    ret = func()
    stop = datetime.now()

    return ret, (stop - start)
