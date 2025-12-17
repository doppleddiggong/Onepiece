#include "PNGCombineManager.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstring> // memcpy


void PNGCombineManager::CombinePNG()
{
    // std::string outputPath = argv[1];
    //
    // // 입력 이미지 경로
    // std::vector<std::string> imagePaths;
    // for (int i = 2; i < argc; ++i) {
    //     imagePaths.push_back(argv[i]);
    // }

    int totalWidth = 0;
    int maxHeight = 0;
    int channels = 0;

    struct ImageData {
        unsigned char* data;
        int width;
        int height;
        int channels;
    };
    std::vector<ImageData> images;

    // PNG 로드
    // for (const auto& path : imagePaths) {
    //     int w, h, c;
    //     // 원하는 채널 수를 4로 강제 (RGBA)하면 다루기 편함
    //     unsigned char* data = stbi_load(path.c_str(), &w, &h, &c, 4);
    //     if (!data) {
    //         std::cerr << "이미지 로드 실패: " << path << "\n";
    //         // 지금까지 로드한 이미지들 해제
    //         for (auto& img : images) {
    //             stbi_image_free(img.data);
    //         }
    //         return -1;
    //     }
    //
    //     // 실제 채널은 4를 사용
    //     c = 4;
    //
    //     images.push_back({ data, w, h, c });
    //
    //     totalWidth += w;
    //     if (h > maxHeight) maxHeight = h;
    //     channels = c; // 어차피 4로 통일
    // }

    int outWidth = totalWidth;
    int outHeight = maxHeight;
    int outChannels = channels; // 4
    size_t outSize = (size_t)outWidth * outHeight * outChannels;

    // 최종 이미지 버퍼
    std::vector<unsigned char> output(outSize, 0);

    // 오른쪽으로 이어 붙이기
    int xOffset = 0;
    for (const auto& img : images) {
        for (int y = 0; y < img.height; ++y) {
            // 목적지 시작 위치
            unsigned char* dstRow = &output[(y * outWidth + xOffset) * outChannels];
            // 소스 시작 위치
            const unsigned char* srcRow = &img.data[y * img.width * img.channels];

            std::memcpy(dstRow, srcRow, (size_t)img.width * outChannels);
        }
        xOffset += img.width;
    }

    // PNG로 저장
    // if (!stbi_write_png(outputPath.c_str(), outWidth, outHeight, outChannels,
    //                     output.data(), outWidth * outChannels)) {
    //     std::cerr << "PNG 저장 실패: " << outputPath << "\n";
    // }

}
