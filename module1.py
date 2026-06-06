#modules that help meassure the latency of the server
import requests
import time
import sys

# How many points will be measured
count = int(sys.argv[1])

#set bot agent up to avoid being blocked by the server and get ttfb
headers = {'User-Agent': 'LatencyProbe/1.0'}

#google http url. Used to make requests for server data
url="https://www.google.com"

successful_measurements = 0

#reuses the tcp session for run time efficiency
session = requests.Session()
session.headers.update(headers)



for attempt in range(count):

    #if program gathers enough data points
    if successful_measurements >= count:
        break

    if successful_measurements > 0 and successful_measurements % 100 == 0:
        session.close()
        session = requests.Session()
        session.headers.update(headers)
    try:
        #seed time counter
        start = time.perf_counter()

        # Issue request. stream=True ensures TTFB is captured at first byte arrival. Gets network http connection from session
        response = session.get(
            url,
            stream=True,
            timeout=10
        )

        #measure ttfb in milliseconds
        ttfb=(time.perf_counter()-start)*1000

        #print the time to first byte with 6 decimal places
        print(f"{ttfb:.6f}", flush=True)

        #close the http connection so the tcp can be reused
        response.close()
        successful_measurements += 1

    # Handle specific exceptions to avoid crashing the program
    except requests.exceptions.Timeout:
        print("TIMEOUT", flush=True)

    except requests.exceptions.ConnectionError:
        print("CONNECTION_ERROR",flush=True)

    #if any other exeption orrurs print it to stderr so the error can be redirected and ignored by the C++ program
    except Exception as e:
        print(f"ERROR: {str(e)}", flush=True)
    
    #give server a 200.5 milisecond break    
    time.sleep(0.2005)