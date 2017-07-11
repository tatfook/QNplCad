local function activate()
   if(msg) then
        NPL.load("(gl)script/ide/commonlib.lua");
        if(msg.start)then
            commonlib.echo("===========================activate");
            NPL.load("(gl)script/ide/timer.lua");

            local mytimer = commonlib.Timer:new({callbackFunc = function(timer)
                commonlib.echo("===========================call");
                NPL.activate("NplCad_ParaEngineApp.cpp", { type = "nplcad", data ="hello world!"})
            end})
            -- start the timer after 0 milliseconds, and signal every 1000 millisecond
            mytimer:Change(0, 10000)
        end
            
   end
end
NPL.this(activate);