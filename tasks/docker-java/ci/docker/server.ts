var express = require("express");
var app = express();
const puppeteer = require('puppeteer')

app.use(express.json({limit: '50mb'}));
app.listen(8080, () => {
    console.log("Puppeteer server is running");
});


app.post("/generate", (req, res) => {
    if (process.env.SHOW_HTML_REPORT && process.env.SHOW_HTML_REPORT === 'true') {
        console.log('Request input:', req.body.html)
    }
    let request: Request = req.body;
    puppeteer.launch({headless: true, args: ['--no-sandbox', '--disable-web-security']}).then(browser => {
        browser.newPage()
            .then(page => {
                    page.setContent(req.body.html, {waitUntil: 'networkidle0'})
                        .then(value => {
                                page.pdf({
                                    format: request.format,
                                    printBackground: true,
                                    displayHeaderFooter: true,
                                    footerTemplate: request.css + footer,
                                    headerTemplate: request.css + header,
                                    margin: {
                                        top: request.pageMarginTop,
                                        bottom: request.pageMarginBottom,
                                        left: request.pageMarginLeft,
                                        right: request.pageMarginRight
                                    }
                                })
                                    .then(generated => {
                                            res.send(generated);
                                        }
                                    ).finally(() => browser.close())
                            }
                        )
                }
            )
    });
});

let footer = `<div class="report-page-footer">
            <span class="report-page-footer-number pageNumber"></span> 
            <span class="report-page-footer-separator">/</span>  
            <span class="report-page-footer-total-pages totalPages"></span>
          </div>`;
let header = `<div class="report-page-header">
            <span class="report-page-header-number pageNumber"></span> 
            <span class="report-page-header-separator">/</span>  
            <span class="report-page-header-total-pages totalPages"></span>
          </div>`;

export class Request {
    html: string
    css: string
    pageMarginTop: string
    pageMarginBottom: string
    pageMarginLeft: string
    pageMarginRight: string
    format: string
}
