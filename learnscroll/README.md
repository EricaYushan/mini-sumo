# LearnScroll (prototype)

A vertical, swipe-to-next feed of short lessons, quizzes, poems and embedded videos.
Not part of the mini-sumo robot — it lives here only until it gets its own repository.

Run it over HTTP (YouTube embeds refuse to play from a `file://` page):

    python3 -m http.server 8000

then open http://localhost:8000/.

- Content is the `CARDS` array in `index.html`. Each topic is an ordered ladder.
- "+ Videos" adds a YouTube link to the feed, or pulls recent Shorts from the
  `CHANNELS` whitelist if you supply a YouTube Data API key. The API path has
  not been tested end to end.
- Progress is stored in the browser's localStorage.
