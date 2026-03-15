# EAE Cloud Model - Usage
-----------------------------------
# FOR RAILWAY PROVIDER (what this project is using)

1. Sign up for Railway. ( [https://railway.app](https://railway.app) )
2. Click New Project -> Pick Node.js from templates.
3. Install Railway CLI. (Windows: `npm install -g railway` )

# IN THE POWERSHELL
4. Enter ( `railway login` ) and login to your Railway account.
5. Enter (` cd EAEAPI` ) to use that subfile for Railway. (it wouldnt work normally if its in a subfile) 
6. Enter ( `railway link` ) to link your project to Railway. (make sure to select the project template you made beforehand and node.js for the service)
7. Enter ( `railway open` ) to open your project.
8. Enter ( `railway up` ) to deploy your project.
9. Enter ( `railway logs` ) to show server logs in real time.
10. Enter ( `railway destroy` ) to undeploy your project.

11. Change the ( `#define CLOUD_ENDPOINT` ) in your config.h to your projects cloud endpoint. (can be found in your projects settings)
- You can enter ( `railway help` ) to check for more commands.