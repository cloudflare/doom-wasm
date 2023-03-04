const saveGameAssets = "saveGameAssets_v0.1.1-alpha"
var iwadfile
var staticfilesPWA = "StaticFiles_v"

const cacheNamePattern = /^StaticFiles_v\d+\.\d+\.\d+(?:-\w+)?$/;

caches.keys().then(cacheNames => {
    cacheNames.forEach(cacheName => {
        if (cacheName.match(cacheNamePattern)) {
            console.log(`Cache ${cacheName}`);
            staticfilesPWA = cacheName
            return
        }
    });
});


// var iwadfile = "freedoom1.wad"


if ("serviceWorker" in navigator) {
    window.addEventListener("load", function () {
        navigator.serviceWorker
            .register("/serviceWorker.js", { scope: '/' })
            .then(res => console.log("service worker registered"))
            .catch(err => console.log("service worker not registered", err));
    });
}



// Get the drop-down menu element
const dropdown = document.getElementById("myDropdown");
const defaultOption = document.createElement("option");
defaultOption.text = "Select a wad file";
defaultOption.disabled = true;
defaultOption.selected = true;
dropdown.add(defaultOption);

// Add an event listener to the drop-down menu
dropdown.addEventListener("click", function () {
    // Get the options from the service worker cache
    caches.open(staticfilesPWA).then(function (cache) {
        cache.keys().then(function (keys) {
            // Create an array of option elements based on the cached HTML files with the extension .wad
            const options = keys
                .filter(function (request) {
                    // Filter the cached HTML files to only those with the extension .wad
                    return request.url.endsWith(".wad");
                })
                .map(function (request) {
                    const option = document.createElement("option");
                    // Get the file name from the URL of the cached HTML file
                    const fileName = request.url.substring(request.url.lastIndexOf('/') + 1);
                    option.value = fileName;
                    option.text = fileName;
                    return option;
                });

            // Remove all existing options from the drop-down menu
            for (let i = dropdown.options.length - 1; i >= 1; i--) {
                dropdown.remove(i);
            }


            // Add the new options to the drop-down menu
            options.forEach(function (option) {
                dropdown.add(option);
            });
        });
    });
});




//showAlert
function showAlert(type = "success", msg = "test", timer = 2000) {
    var alertContainer = document.getElementById("alert-container");
    alertContainer.style.display = "block";
    document.getElementById("alert-message").innerHTML = msg;

    if (type === "success") {
        alertContainer.style.backgroundColor = "#d4edda";
        alertContainer.style.color = "black";
        document.getElementById("alert-message-header").innerHTML = "Success!";
        document.getElementById("alert-message-header").style.color = "green";
    } else if (type === "error") {
        alertContainer.style.backgroundColor = "#f8d7da";
        alertContainer.style.color = "black";
        document.getElementById("alert-message-header").innerHTML = "Error!";
        document.getElementById("alert-message-header").style.color = "red";
    }

    setTimeout(function () {
        alertContainer.style.animation = "hidePopup 0.5s ease-in-out forwards";
        setTimeout(function () {
            alertContainer.style.display = "none";
            alertContainer.style.animation = "showPopup 0.5s ease-in-out forwards";
        }, 500);
    }, timer);
}


// Ctrl + S to Save Savefile to Cache
document.addEventListener("keydown", function (event) {
    if (event.ctrlKey && event.code === "KeyS") {
        event.preventDefault();
        saveFileSystemDataToCache();
    }
});


async function saveFileSystemDataToCache() {
    console.group("[Actived] Saving File System Data To Cache")
    // Open the cache
    const cache = await caches.open(saveGameAssets);

    // Get all the files in the `/savefiles/` directory
    const files = Module.FS.readdir("/savefiles/").slice(2);;

    // Create an array of promises to cache each file
    const cachePromises = files.map(async file => {
        console.log(`Saving files...${file}`)
        // Read the file data using Emscripten's file system API
        const data = Module.FS.readFile(`/savefiles/${file}`, { encoding: 'binary' });
        // Put the file data in the cache
        return cache.put(`/savefiles/${file}`, new Response(data, { headers: { 'Content-Type': 'application/octet-stream' } }));
    });

    // Wait for all the files to be cached
    await Promise.all(cachePromises).then(() => {
        showAlert("success", "Game Savefiles successfuly Saved!", 2000);
    })
        .catch((error) => {
            showAlert("error", "While saving savefile data!", 2000);
            console.warn(`Error: While saving savefile data! ${error}`)
        });

    console.groupEnd()
}


async function retrieveFileSystemDataFromCache() {
    console.group("[Active] File System Data Retrieval From Cache")
    // Open the cache
    const cache = await caches.open(saveGameAssets);

    // Find all files in the cache with the pattern `doomsav0.dsg,doomsav1.dsg,doomsav2.dsg`
    const cacheFiles = await cache.keys();
    const savedFiles = cacheFiles.filter(request => request.url.includes("doomsav") && request.url.endsWith(".dsg"));

    // Create an array of promises to retrieve each file from the cache
    const retrievePromises = savedFiles.map(async request => {
        // Get the file from the cache
        const response = await cache.match(request);
        const data = await response.arrayBuffer();

        // Write the file data to disk using Emscripten's file system API
        const filePath = `/savefiles/${request.url.substring(request.url.lastIndexOf("/") + 1)}`;
        console.log("Retrieve files...", request.url.substring(request.url.lastIndexOf("/") + 1))
        Module.FS.writeFile(filePath, new Uint8Array(data), { encoding: 'binary' });
    });


    // Wait for all the files to be retrieved from the cache and written to disk
    await Promise.all(retrievePromises)
        .then(() => {
            showAlert("success", "Game Savefiles successfuly Retrieved!", 2000);
        })
        .catch((error) => {
            showAlert("error", "While retrieving savefile data!", 2000);
            console.warn(`Error: While retrieving savefile data! ${error}`)
        });
    console.groupEnd()
}



var Module = {
    onRuntimeInitialized: () => {
        dropdown.addEventListener("change", function () {
            // Get the selected value
            iwadfile = dropdown.value;
            var commonArgs = ["-iwad", `${iwadfile}`, "-window", "-nogui", "-nomusic", "-config", "default.cfg", "-servername", "doomflare", "-savedir", "/savefiles", "-autojoin", " -left", "-window"]

            console.log(commonArgs)
            const canvas = document.getElementById("canvas");
            // canvas.oncontextmenu = event.preventDefault()
            canvas.tabIndex = "-1"
            canvas.style.display = "block"


            callMain(commonArgs);

        });


    },
    noInitialRun: true,
    preRun: () => {
        console.log(`preRun----`)
        caches.open(staticfilesPWA).then(function (cache) {
            cache.keys().then(function (keys) {
                // Create an array of option elements based on the cached HTML files with the extension .wad
                const options = keys
                    .filter(function (request) {
                        // Filter the cached HTML files to only those with the extension .wad
                        return request.url.endsWith(".wad");
                    })
                    .map(function (request) {
                        // Get the file name from the URL of the cached HTML file
                        const fileName = request.url.substring(request.url.lastIndexOf('/') + 1);
                        console.log(fileName)
                        Module.FS.createPreloadedFile("", `${fileName}`, `${fileName}`, true, true);
                    });
            })
        })

        Module.FS.createPreloadedFile("", "default.cfg", "default.cfg", true, true);

    },
    postRun: async () => { await retrieveFileSystemDataFromCache(); },

    printErr: function (text) {
        if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(" ");
        console.error(text);
    },
    canvas: (function () {
        var canvas = document.getElementById("canvas");
        canvas.addEventListener(
            "webglcontextlost",
            function (e) {
                alert("WebGL context lost. You will need to reload the page.");
                e.preventDefault();
            },
            false
        );
        return canvas;
    })(),
    print: function (text) {
        console.log(text);
    },
    setStatus: function (text) {
        console.log(text);
    },
    totalDependencies: 0,
    monitorRunDependencies: function (left) {
        this.totalDependencies = Math.max(this.totalDependencies, left);
        Module.setStatus(left ? "Preparing... (" + (this.totalDependencies - left) + "/" + this.totalDependencies + ")" : "All downloads complete.");
    },
};


window.onerror = function (event) {
    Module.setStatus("Exception thrown, see JavaScript console");
    Module.setStatus = function (text) {
        if (text) Module.printErr("[post-exception status] " + text);
    };
};





//Footer
let text = [
    "W,A,S,D OR ARROWS TO MOVE, Q,E,Z|O TO STRAFE, X|P FOR SPEED, C TO OPEN, TAB SHOWS THE MAP ",
    "YOU CAN ALSO USE THE MOUSE, CTRL + S COPY SAVEFILE TO CACHE (SO YOU CAN PLAY NEXT TIME)"
];
let currentText = 0;
let speed = 2;
let footer = document.getElementById("footer");
footer.innerHTML = text[currentText];

let x = footer.offsetWidth * 2;

function animate() {
    x -= speed;
    if (x < -footer.offsetWidth) {
        x = footer.offsetWidth * 2;
        currentText = (currentText + 1) % text.length;
        footer.innerHTML = text[currentText];
    }
    footer.style.transform = "translate(" + x + "px, 0)";
    requestAnimationFrame(animate);
}
animate();
