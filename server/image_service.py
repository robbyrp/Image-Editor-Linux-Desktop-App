from fastapi import Response
from httpx import AsyncClient
from multiprocessing import shared_memory
from io import BytesIO
from PIL import Image
import asyncio

MIN_COLS_PIXELS = 300
MIN_ROWS_PIXELS = 300

client = AsyncClient()

class ImageService:
    async def get_random_breed_dog_image_ppm(self) -> bytes:
        RANDOM_ALL_BREEDS_URL = "https://dog.ceo/api/breeds/image/random"
        response = await client.get(RANDOM_ALL_BREEDS_URL)
        download_url = response.json()["message"]
        print("Url-ul primit de la dogAPI " + download_url)

        response = await client.get(download_url)
        # The get request returns a JPEG image, that is stored in a BytesIO buffer.
        jpeg_buffer = BytesIO(response.content)
        # The JPEG image is opened so that it can be converted to PPM format.
        image = Image.open(jpeg_buffer, mode="r")
        # The converted PPM image needs to be stores in a BytesIO buffer so thaat it can be returned as a byte stream
        ppm_buffer = BytesIO()
        image.save(ppm_buffer, format="PPM")
        return ppm_buffer.getvalue()
    
    # Makes a GET request to an endpoint of list_length dog images, returned in a list
    async def get_download_urls(self, list_length: int) -> list:
        RANDOM_ALL_BREEDS_URL = f"https://dog.ceo/api/breeds/image/random/{list_length}"
        response = await client.get(RANDOM_ALL_BREEDS_URL)
        download_urls = response.json()["message"]
        return download_urls            
    
    # Returns the (rows, cols) pair by reading chunks of the http get response using httpx stream()
    async def _get_image_sizes_from_url(self, url: str) -> tuple[int, int]:
        async with client.stream("GET", url) as response:
            if response.status_code != 200:
                raise Exception(f"Failed accessing URL: {response.status_code}")
        
            buffer = bytearray()
            async for chunk in response.aiter_bytes(chunk_size=1024):
                buffer.extend(chunk)
                
                try:
                    image = Image.open(BytesIO(buffer))
                    # Returns a (width, height) tuple
                    size = image.size
                    await response.aclose()
                    return size
                except(IOError, SyntaxError):
                    if len(buffer) > 10240:
                        break
                
        raise Exception("Could not read image size from the first 10KB of the header")

    
    # Constructs the (url->(rows, cols)) dictionary by running the 9 get requests concurrently using asyncio.gather()
    async def _generate_url_and_sizes_dictionary(self, download_urls: str) -> dict[str, tuple[int, int]]:
        table: dict[str, tuple[int, int]] = {}
        tasks: list = []
        
        for url in download_urls:
            task =self._get_image_sizes_from_url(url)
            tasks.append(task)
        
        results: list[tuple[int, int]] = await asyncio.gather(*tasks)

        num_of_urls: int = len(download_urls)
        for i in range (num_of_urls):
            url: str = download_urls[i]
            image_size: tuple[int, int] = results[i]
            
            table[url] = image_size
          
        return table  
                    
    # Returns the url of the best sized image
    async def get_best_sized_image_url(self, download_urls: str) -> str:
        table = await self._generate_url_and_sizes_dictionary(download_urls)
        
        max_product: int = -1
        best_url: str = ""
        for (url, (rows, cols)) in table.items():
             if (cols > MIN_COLS_PIXELS and rows > MIN_ROWS_PIXELS):
                 if (max_product < cols * rows):
                     max_product = cols * rows
                     best_url = url
        return best_url
    
    # Returns the name of the shared memory block
    async def map_image_from_url_to_shared_memory(self, best_url: str) -> str:
        response = await client.get(best_url)

        jpeg_buffer = BytesIO(response.content)

        image = Image.open(jpeg_buffer, mode="r")

        ppm_buffer = BytesIO()
        image.save(ppm_buffer, format="PPM")
        
        ppm_image_size = ppm_buffer.getbuffer().nbytes
        
        shm_image_buffer = shared_memory.SharedMemory(create=True,
                                                      size=ppm_image_size)
        shm_image_buffer.buf[:ppm_image_size] = ppm_buffer.getbuffer()
        return shm_image_buffer.name
    
