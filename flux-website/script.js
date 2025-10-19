// Flux Programming Language Website JavaScript
document.addEventListener('DOMContentLoaded', function() {
    // Smooth scrolling for navigation links
    document.querySelectorAll('a[href^="#"]').forEach(anchor => {
        anchor.addEventListener('click', function (e) {
            e.preventDefault();
            const target = document.querySelector(this.getAttribute('href'));
            if (target) {
                target.scrollIntoView({
                    behavior: 'smooth',
                    block: 'start'
                });
            }
        });
    });

    // Download functionality
    const downloadButtons = document.querySelectorAll('#downloadBtn, #mainDownloadBtn');
    const zipFilePath = './flux-lang.zip'; // Relative path to the ZIP file
    
    // Check if ZIP file exists and update file size
    checkFileSize();
    
    downloadButtons.forEach(button => {
        button.addEventListener('click', function() {
            downloadFile();
        });
    });

    function downloadFile() {
        try {
            // Create a temporary link element
            const link = document.createElement('a');
            link.href = zipFilePath;
            link.download = 'flux-lang.zip';
            
            // Add the link to the document, click it, then remove it
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
            
            // Show download success message
            showNotification('Download started! flux-lang.zip should begin downloading.', 'success');
            
            // Track download (could be used for analytics)
            trackDownload();
            
        } catch (error) {
            console.error('Download error:', error);
            showNotification('Download failed. Please try again or check the file path.', 'error');
        }
    }
    
    async function checkFileSize() {
        try {
            const response = await fetch(zipFilePath, { method: 'HEAD' });
            if (response.ok) {
                const contentLength = response.headers.get('content-length');
                if (contentLength) {
                    const sizeInBytes = parseInt(contentLength);
                    const sizeInKB = Math.round(sizeInBytes / 1024);
                    const sizeInMB = (sizeInBytes / (1024 * 1024)).toFixed(1);
                    
                    const fileSizeElement = document.getElementById('fileSize');
                    if (fileSizeElement) {
                        if (sizeInKB < 1024) {
                            fileSizeElement.textContent = `${sizeInKB}KB`;
                        } else {
                            fileSizeElement.textContent = `${sizeInMB}MB`;
                        }
                    }
                }
            }
        } catch (error) {
            console.log('Could not determine file size:', error);
        }
    }
    
    function trackDownload() {
        // Track download event (could integrate with analytics)
        console.log('Flux download initiated:', new Date().toISOString());
        
        // You could add analytics tracking here:
        // gtag('event', 'download', { 'file_name': 'flux-lang.zip' });
        // or
        // analytics.track('File Downloaded', { filename: 'flux-lang.zip' });
    }
    
    function showNotification(message, type = 'info') {
        // Create notification element
        const notification = document.createElement('div');
        notification.className = `notification notification-${type}`;
        notification.innerHTML = `
            <div class="notification-content">
                <span class="notification-icon">${type === 'success' ? '✅' : '❌'}</span>
                <span class="notification-message">${message}</span>
                <button class="notification-close">&times;</button>
            </div>
        `;
        
        // Add styles
        notification.style.cssText = `
            position: fixed;
            top: 20px;
            right: 20px;
            background: ${type === 'success' ? '#10b981' : '#ef4444'};
            color: white;
            padding: 1rem 1.5rem;
            border-radius: 8px;
            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);
            z-index: 10000;
            max-width: 400px;
            transform: translateX(100%);
            transition: transform 0.3s ease;
        `;
        
        const content = notification.querySelector('.notification-content');
        content.style.cssText = `
            display: flex;
            align-items: center;
            gap: 0.5rem;
        `;
        
        const closeBtn = notification.querySelector('.notification-close');
        closeBtn.style.cssText = `
            background: none;
            border: none;
            color: white;
            font-size: 1.2rem;
            cursor: pointer;
            margin-left: auto;
            padding: 0;
        `;
        
        // Add to document
        document.body.appendChild(notification);
        
        // Animate in
        setTimeout(() => {
            notification.style.transform = 'translateX(0)';
        }, 10);
        
        // Close functionality
        closeBtn.addEventListener('click', () => {
            removeNotification(notification);
        });
        
        // Auto-remove after 5 seconds
        setTimeout(() => {
            if (notification.parentNode) {
                removeNotification(notification);
            }
        }, 5000);
    }
    
    function removeNotification(notification) {
        notification.style.transform = 'translateX(100%)';
        setTimeout(() => {
            if (notification.parentNode) {
                notification.parentNode.removeChild(notification);
            }
        }, 300);
    }
    
    // Add some visual feedback for buttons
    downloadButtons.forEach(button => {
        button.addEventListener('mouseenter', function() {
            this.style.transform = 'translateY(-2px) scale(1.02)';
        });
        
        button.addEventListener('mouseleave', function() {
            this.style.transform = 'translateY(0) scale(1)';
        });
        
        button.addEventListener('mousedown', function() {
            this.style.transform = 'translateY(0) scale(0.98)';
        });
        
        button.addEventListener('mouseup', function() {
            this.style.transform = 'translateY(-2px) scale(1.02)';
        });
    });
    
    // Add scroll effect to header
    window.addEventListener('scroll', function() {
        const header = document.querySelector('.header');
        if (window.scrollY > 100) {
            header.style.background = 'rgba(255, 255, 255, 0.98)';
            header.style.boxShadow = '0 2px 20px rgba(0, 0, 0, 0.1)';
        } else {
            header.style.background = 'rgba(255, 255, 255, 0.95)';
            header.style.boxShadow = 'none';
        }
    });
    
    // Add fade-in animation for feature cards
    const observerOptions = {
        threshold: 0.1,
        rootMargin: '0px 0px -50px 0px'
    };
    
    const observer = new IntersectionObserver(function(entries) {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.style.opacity = '1';
                entry.target.style.transform = 'translateY(0)';
            }
        });
    }, observerOptions);
    
    // Apply animation to cards
    document.querySelectorAll('.feature-card, .docs-card, .code-example').forEach(card => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(20px)';
        card.style.transition = 'opacity 0.6s ease, transform 0.6s ease';
        observer.observe(card);
    });
    
    // Mobile menu toggle (if needed)
    const navLinks = document.querySelector('.nav-links');
    if (window.innerWidth <= 768) {
        navLinks.addEventListener('click', function(e) {
            if (e.target.tagName === 'A') {
                // Close mobile menu when link is clicked
                // This would be useful if you add a hamburger menu
            }
        });
    }
    
    // Console easter egg
    console.log(`
    ⚡ Flux Programming Language ⚡
    
    Welcome to the Flux website!
    This is a demonstration programming language built with:
    - Lexer/Tokenizer
    - Recursive Descent Parser  
    - Tree-walking Interpreter
    - Dynamic typing & closures
    
    Built with modern C++17 and love 💜
    
    Check out the source code after downloading!
    `);
});