import subprocess
import time
import os
import signal
import json
import statistics

def get_process_tree(root_pid):
    try:
        output = subprocess.check_output(['ps', '-e', '-o', 'pid,ppid,rss,%cpu,comm']).decode('utf-8')
    except subprocess.CalledProcessError:
        return []
    
    processes = []
    for line in output.splitlines()[1:]:
        parts = line.split(maxsplit=4)
        if len(parts) == 5:
            processes.append({
                'pid': int(parts[0]),
                'ppid': int(parts[1]),
                'rss': int(parts[2]),
                'cpu': float(parts[3]),
                'comm': parts[4].strip()
            })
            
    tree_pids = {root_pid}
    added = True
    while added:
        added = False
        for p in processes:
            if p['ppid'] in tree_pids and p['pid'] not in tree_pids:
                tree_pids.add(p['pid'])
                added = True
                
    result = []
    for p in processes:
        if p['pid'] in tree_pids:
            result.append(p)
    return result

def run_benchmark(is_cold):
    if is_cold:
        # Best effort drop caches if we had sudo, but we don't.
        # Just sleep to let OS settle, or run a dummy find command to trash cache (optional).
        time.sleep(2)
        
    start_time = time.time()
    proc = subprocess.Popen(['./daemon_browser', '--url=https://example.com/'], 
                            cwd='build',
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    startup_time = None
    load_end_time = None
    
    while True:
        line = proc.stdout.readline()
        if not line:
            break
        if "BENCHMARK_SIGNAL: STARTUP_READY" in line and startup_time is None:
            startup_time = time.time() - start_time
        if "BENCHMARK_SIGNAL: LOAD_END" in line and load_end_time is None:
            load_end_time = time.time() - start_time
            break
            
    # Wait for stabilization
    time.sleep(2)
    
    # Measure memory and CPU
    tree = get_process_tree(proc.pid)
    total_rss = sum(p['rss'] for p in tree)
    main_rss = next((p['rss'] for p in tree if p['pid'] == proc.pid), 0)
    process_count = len(tree)
    
    # Count by comm
    breakdown = {}
    for p in tree:
        comm = p['comm']
        breakdown[comm] = breakdown.get(comm, 0) + 1
        
    # Shutdown
    shutdown_start = time.time()
    proc.terminate()
    try:
        proc.wait(timeout=5)
    except subprocess.TimeoutExpired:
        proc.kill()
    shutdown_time = time.time() - shutdown_start
    
    return {
        'startup_time': startup_time,
        'load_end_time': load_end_time,
        'total_rss_kb': total_rss,
        'main_rss_kb': main_rss,
        'process_count': process_count,
        'breakdown': breakdown,
        'shutdown_time': shutdown_time
    }

print("Running Cold Starts...")
cold_starts = []
for i in range(5):
    res = run_benchmark(is_cold=True)
    print(f"  Cold Run {i+1}: Startup {res['startup_time']:.3f}s, Load {res['load_end_time']:.3f}s")
    cold_starts.append(res)

print("Running Warm Starts...")
warm_starts = []
for i in range(5):
    res = run_benchmark(is_cold=False)
    print(f"  Warm Run {i+1}: Startup {res['startup_time']:.3f}s, Load {res['load_end_time']:.3f}s")
    warm_starts.append(res)

print("\n--- RESULTS ---")
print(json.dumps({
    'cold': cold_starts,
    'warm': warm_starts
}, indent=2))
