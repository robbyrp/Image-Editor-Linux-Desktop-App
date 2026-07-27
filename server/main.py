from fastapi import FastAPI
from fastapi import Response
from pydantic import BaseModel
from image_service import ImageService

app = FastAPI()
service = ImageService()

@app.get("/")
async def root() -> dict:
    return {"message": "Hello World"}

@app.get("/api/random-breed")
async def all_dogs() -> Response:
    image = await service.get_random_breed_dog_image_ppm()
    return Response(content=image, media_type="image/x-portable-pixmap")

@app.get("/api/{breed}")
async def breed_random(breed: str) -> Response:
    image = await service.get_dog_breed_image_ppm(breed)
    return Response(content=image, media_type="image/x-portable-pixmap")


LIST_LENGTH = 9
@app.get("/api/random-breed/{LIST_LENGTH}")
#   Returns a list of LIST_LENGTH download url's
async def random_breed_list() -> list:
    download_urls = await service.get_download_urls(LIST_LENGTH)
    return download_urls

@app.get("/api/thumbnail}")
async def get_thumbnail(download_url: str) -> Response:
    thumbnail = await service._create_thumbnail(download_url)
    return Response(content=thumbnail, media_type="image/x-portable-pixmap")