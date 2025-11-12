#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

// ORB 영상 정합
int main()
{
    // ===== 1. 이미지 로드 =====
    cout << "[1단계] 이미지 로드" << endl;

    Mat img_ori = imread("ori.png", IMREAD_GRAYSCALE);
    Mat img_input = imread("input.png", IMREAD_GRAYSCALE);

    // 로드 확인
    if (img_ori.empty() || img_input.empty())
    {
        cout << "[오류] 이미지 로드 불가!" << endl;
        cout << "ori.png와 input.png 파일을 프로젝트 폴더에 넣어주세요." << endl;
        cout << "현재 경로: " << endl;
        system("cd");
        return -1;
    }

    cout << "원본 영상 크기: " << img_ori.cols << " x " << img_ori.rows << endl;
    cout << "입력 영상 크기: " << img_input.cols << " x " << img_input.rows << endl;
    cout << endl;

    // ===== 2. ORB 검출기 생성 =====
    cout << "[2단계] ORB 검출기 생성" << endl;

    // ORB 파라미터 설정
    int nfeatures = 500;      // 최대 특징점 개수
    float scaleFactor = 1.2f;  // 스케일 피라미드 비율
    int nlevels = 8;           // 피라미드 레벨 수

    Ptr<ORB> orb = ORB::create(nfeatures, scaleFactor, nlevels);

    cout << "ORB 설정:" << endl;
    cout << "  - 최대 특징점: " << nfeatures << "개" << endl;
    cout << "  - 스케일 팩터: " << scaleFactor << endl;
    cout << "  - 피라미드 레벨: " << nlevels << endl;
    cout << endl;

    // ===== 3. 특징점 검출 =====
    cout << "[3단계] 특징점 검출" << endl;

    vector<KeyPoint> keypoints_ori, keypoints_input;

    orb->detect(img_ori, keypoints_ori);
    orb->detect(img_input, keypoints_input);

    cout << "원본 영상 특징점: " << keypoints_ori.size() << "개" << endl;
    cout << "입력 영상 특징점: " << keypoints_input.size() << "개" << endl;
    cout << endl;

    // ===== 4. 기술자 계산 =====
    cout << "[4단계] 기술자 계산" << endl;

    Mat descriptors_ori, descriptors_input;

    orb->compute(img_ori, keypoints_ori, descriptors_ori);
    orb->compute(img_input, keypoints_input, descriptors_input);

    cout << "원본 기술자 크기: " << descriptors_ori.rows << " x "
        << descriptors_ori.cols << endl;
    cout << "입력 기술자 크기: " << descriptors_input.rows << " x "
        << descriptors_input.cols << endl;
    cout << "기술자 타입: Binary (256-bit)" << endl;
    cout << endl;

    // ===== 5. 매칭 수행 =====
    cout << "[5단계] 매칭 수행" << endl;

    // BFMatcher with Hamming distance (ORB는 이진 기술자)
    BFMatcher matcher(NORM_HAMMING);
    vector<DMatch> matches;

    matcher.match(descriptors_ori, descriptors_input, matches);

    cout << "총 매칭 개수: " << matches.size() << "개" << endl;
    cout << endl;

    // ===== 6. 좋은 매칭만 선택 =====
    cout << "[6단계] 좋은 매칭 필터링" << endl;

    // 거리 기준 정렬
    sort(matches.begin(), matches.end());

    // 최소 거리 계산
    double min_dist = matches[0].distance;
    double max_dist = matches[matches.size() - 1].distance;

    cout << "최소 매칭 거리: " << min_dist << endl;
    cout << "최대 매칭 거리: " << max_dist << endl;

    // 좋은 매칭: 최소 거리의 2배 이하
    vector<DMatch> good_matches;
    double distance_threshold = min_dist * 2;

    for (size_t i = 0; i < matches.size(); i++)
    {
        if (matches[i].distance <= distance_threshold)
        {
            good_matches.push_back(matches[i]);
        }
    }

    // 최소 25개는 유지
    if (good_matches.size() < 25)
    {
        good_matches.clear();
        for (size_t i = 0; i < min(25, (int)matches.size()); i++)
        {
            good_matches.push_back(matches[i]);
        }
    }

    cout << "선택된 좋은 매칭: " << good_matches.size() << "개" << endl;
    cout << "매칭 성공률: "
        << (double)good_matches.size() / keypoints_ori.size() * 100
        << "%" << endl;

    // 평균 거리 계산
    double avg_distance = 0;
    for (const auto& match : good_matches)
    {
        avg_distance += match.distance;
    }
    avg_distance /= good_matches.size();

    cout << "평균 매칭 거리: " << avg_distance << endl;
    cout << endl;

    // ===== 7. 결과 시각화 =====
    cout << "[7단계] 결과 시각화" << endl;

    Mat img_matches;
    drawMatches(img_ori, keypoints_ori,
        img_input, keypoints_input,
        good_matches, img_matches,
        Scalar::all(-1), Scalar::all(-1),
        vector<char>(),
        DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // 정보 텍스트 추가
    string info = "ORB Matches: " + to_string(good_matches.size());
    putText(img_matches, info, Point(10, 30),
        FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 0), 2);

    // 결과 출력
    imshow("ORB Feature Matching", img_matches);

    waitKey(0);

    return 0;
}