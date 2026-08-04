from fastapi import FastAPI
from fastapi import Response
from image_service import ImageService

app = FastAPI()
service = ImageService()

@app.get("/")
async def root() -> dict:
    return {"message": "Hello World"}

# Used for mvp button
@app.get("/api/random-breed")
async def all_dogs() -> Response:
    image = await service.get_random_breed_dog_image_ppm()
    return Response(content=image, media_type="image/x-portable-pixmap")


@app.get("/api/random-breed/best-sized-image-from-list")
#   Returns the best sized-list image in PPM format from a list of 9 urls
async def get_best_sized_image_from_list(length: int = 9) -> str:
    download_urls = await service.get_download_urls(length)
    best_url = await service.get_best_sized_image_url(download_urls)
    shm_name_key = await service.map_image_from_url_to_shared_memory(best_url)
    return shm_name_key
