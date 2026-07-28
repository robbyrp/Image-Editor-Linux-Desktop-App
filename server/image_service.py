from fastapi import Response
from httpx import AsyncClient
from io import BytesIO
from PIL import Image

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
    
    async def get_dog_breed_image_ppm(self, breed: str) -> bytes:
        SPECIFIC_BREED_URL = f"https://dog.ceo/api/breed/{breed}/images/random"
        response = await client.get(SPECIFIC_BREED_URL)
        download_url = response.json()["message"]
        
        response = await client.get(download_url)
        # The get request returns a JPEG image, that is stored in a BytesIO buffer.
        jpeg_buffer = BytesIO(response.content)
        # The JPEG image is opened so that it can be converted to PPM format.
        image = Image.open(jpeg_buffer, mode="r")
        # The converted PPM image needs to be stores in a BytesIO buffer so thaat it can be returned as a byte stream
        ppm_buffer = BytesIO()
        image.save(ppm_buffer, format="PPM")
        return ppm_buffer.getvalue()
    
    async def get_download_urls(self, list_length: int) -> list:
        RANDOM_ALL_BREEDS_URL = f"https://dog.ceo/api/breeds/image/random/{list_length}"
        response = await client.get(RANDOM_ALL_BREEDS_URL)
        download_urls = response.json()["message"]
        return download_urls        
        
    async def _create_thumbnail(self, download_url) -> bytes:
        response = await client.get(download_url)
        
        jpeg_buffer = BytesIO(response.content)
        image = Image.open(jpeg_buffer, mode="r")
        
        thumbnail_buffer = BytesIO()
        THUMBNAIL_SIZE = 156
        image.thumbnail((THUMBNAIL_SIZE, THUMBNAIL_SIZE))
        image.save(thumbnail_buffer, format="PPM")
        return thumbnail_buffer.getvalue()
        