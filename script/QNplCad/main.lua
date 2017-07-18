local function activate()
   if(msg) then
        NPL.load("(gl)script/ide/commonlib.lua");
        NPL.load("(gl)script/ide/serialization.lua");
        NPL.load("(gl)script/ide/Json.lua");
        commonlib.echo("===========msg");
        commonlib.echo(msg);
        if(msg.build and msg.filename)then
            NPL.load("Mod/NplCadLibrary/services/CSGService.lua");
            local CSGService = commonlib.gettable("Mod.NplCadLibrary.services.CSGService");
            local filename = msg.filename;
            local output = CSGService.build(filename,true)
            commonlib.echo("========output");
            commonlib.echo(output);
            local msg = commonlib.Json.Encode({ type = "qnplcad_output", data = output, test = {1,2,3}, })
            NPL.activate("NplCad_ParaEngineApp.cpp", msg);
        end
            
   end
end
NPL.this(activate);