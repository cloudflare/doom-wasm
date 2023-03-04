const BasePath = ''
const staticfilesPWA = "StaticFiles_v0.1.3-alpha"

const assets = [
    `${BasePath}/`,
    `${BasePath}/static/js/app.js`,
    `${BasePath}/static/images/icons/chocolate-doom_icon_x512.png`,
    `${BasePath}/static/images/icons/chocolate-doom_icon_x192.png`,

    `${BasePath}/websockets-doom.js`,
    `${BasePath}/default.cfg`,
    `${BasePath}/websockets-doom.wasm`,

    "https://fonts.gstatic.com/s/vt323/v12/pxiKyp0ihIEF2isfFJXUdVNF.woff2",
]

const wadFiles = [
    `${BasePath}/doom1.wad`,
    `${BasePath}/doom2.wad`,
    `${BasePath}/freedoom1.wad`,
    `${BasePath}/freedoom2.wad`,
];



// self.addEventListener("install", installEvent => {
//     console.log("service worker installed")
//     installEvent.waitUntil(
//         caches.open(staticfilesPWA).then(cache => {
//             cache.addAll(assets).then(r => {
//                 console.log("Cache assets downloaded");
//             }).catch(err => console.log("Error caching item", err))
//             console.log(`Cache ${staticfilesPWA} opened.`);
//         }).catch(err => console.log("Error opening cache", err))
//     )
// })

self.addEventListener("install", installEvent => {
    console.log("service worker installed");
    installEvent.waitUntil(
        Promise.all([
            caches.open(staticfilesPWA).then(cache => {
                return cache.addAll(assets);
            }),
            Promise.all(wadFiles.map(file => {
                return fetch(file)
                    .then(res => {
                        if (res.ok) {
                            return caches.open(staticfilesPWA)
                                .then(cache => cache.put(file, res));
                        }
                    })
                    .catch(err => console.log(`Error checking for file ${file}: `, err));
            }))
        ]).then(() => {
            console.log(`Cache ${staticfilesPWA} opened.`);
            console.log("Cache assets downloaded");
        }).catch(err => console.log("Error caching items", err))
    );
});


// activate event
self.addEventListener('activate', evt => {
    console.log('service worker activated');
    evt.waitUntil(
        caches.keys().then(keys => {
            return Promise.all(keys
                .filter(key => key !== staticfilesPWA)
                .map(key => caches.delete(key))
            );
        })
    );
});


self.addEventListener("fetch", fetchEvent => {
    console.log("fetched", fetchEvent.request.url)
    fetchEvent.respondWith(
        caches.match(fetchEvent.request).then(res => {
            return res || fetch(fetchEvent.request)
        }).catch(err => console.log("Cache fetch error: ", err))
    )
})