-- Simple Lua Script

-- Get the starting time
local startTime

function onMount()
    log("info", "mounted controller\n")
    startTime = os.time()
end

function onUpdate()
    local currentTime = os.time()

    local elapsedTime = currentTime - startTime

    if elapsedTime >= 3 then
        log("info", "Splash over, closing game.\n")
        loadScene("scenes/mainmenu.yoyo")
        -- exitGame()
    end
end

function onUnmount()
    log("info", "unmounted controller\n")
end
