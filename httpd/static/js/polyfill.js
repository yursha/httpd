/*********   POLYFILLS   **********
 * 
 * Browser compatibility of each function may be checked at MDN website.
 * 
 * TODO: In the future we may detect user's client on each incoming request
 * and serve only those polyfills which are needed.
 */

Number.isFinite = Number.isFinite || function(value) {
    return typeof value === 'number' && isFinite(value);
}
